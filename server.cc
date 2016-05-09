#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"
#include "peer.cpp"

#include "processor.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using onevote::Empty;
using onevote::BlockMsg;
using onevote::TransactionMsg;
using onevote::AddrRequest;
using onevote::AddrResponse;
using onevote::TransactionRequest;
using onevote::BlockRequest;
using onevote::Miner;

transaction* decode_transaction(const TransactionMsg* transaction_msg) {
  transaction* decoded_transaction = new transaction;

  decoded_transaction->sender_public_key = transaction_msg->sender_public_key();
  decoded_transaction->vote              = transaction_msg->vote();
  decoded_transaction->timestamp         = transaction_msg->timestamp();

  return decoded_transaction;
}

TransactionMsg* encode_transaction(transaction* transaction) {
  TransactionMsg* encoded_transaction = new TransactionMsg;

  encoded_transaction->set_sender_public_key(transaction->sender_public_key);
  encoded_transaction->set_vote(transaction->vote);
  encoded_transaction->set_timestamp(transaction->timestamp);

  return encoded_transaction;
}

block* decode_block(BlockMsg* block_msg) {
  block* decoded_block = new block;

  decoded_block->block_number        = block_msg->block_number();
  decoded_block->prev_block_SHA1     = (char*) block_msg->prev_block_sha1().c_str();
  decoded_block->magic               = block_msg->magic();
  decoded_block->merkle_root         = (char*) block_msg->merkle_root().c_str();
  decoded_block->max_ind             = block_msg->num_transactions();
  decoded_block->verifier_public_key = (char*) block_msg->verifier_public_key().c_str();
  decoded_block->finhash             = (char*) block_msg->final_hash().c_str();

  for (int i = 0; i < block_msg->transaction_msg_size(); i++) {
    decoded_block->transaction_array[i] = decode_transaction(&block_msg->transaction_msg(i));
  }

  return decoded_block;
}

BlockMsg* encode_block(block* block) {
  BlockMsg* encoded_block = new BlockMsg;

  encoded_block->set_block_number(block->block_number);
  encoded_block->set_prev_block_sha1(block->prev_block_SHA1);
  encoded_block->set_magic(block->magic);
  encoded_block->set_merkle_root(block->merkle_root);
  encoded_block->set_num_transactions(block->max_ind);
  encoded_block->set_verifier_public_key(block->verifier_public_key);
  encoded_block->set_final_hash(block->finhash);

  for (int i = 0; i < block->max_ind; i++) {
    TransactionMsg* transaction_msg = encoded_block->add_transaction_msg();

    transaction_msg->set_sender_public_key(block->transaction_array[i]->sender_public_key);
    transaction_msg->set_vote(block->transaction_array[i]->vote);
    transaction_msg->set_timestamp(block->transaction_array[i]->timestamp);
  }

  return encoded_block;
}

// Logic and data behind the server's behavior.
class MinerServiceImpl final : public Miner::Service {
public:
  MinerServiceImpl(comm_thread_args* ctap, Client* client) : Miner::Service {
    ctap_ = ctap;
    client_ = client;
  }

	Status BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) override {
    ctap->bq->push(decode_block(block_msg));
		return Status::OK;
	}

	Status BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) override {
    ctap->tq->push(decode_transaction(transaction_msg));
		return Status::OK;
	}

	Status GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp) override {
    for (const auto& peer: client_->getPeersList()) {
      addr_resp->add_peer(*peer);
    }
    return Status::OK;
	}

	Status GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg) override {
    // TODO not going to implement this one
    return Status::CANCELLED;
	}

	Status GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg) override {
    if (block_req->get_block_number() == NULL) {
      *block_msg = *encode_block(ctap->bc->get_head_block());
    } else {
      Block* block = ctap->bc->get_block(block_req->get_block_number());
      if (block == NULL) {
        return Status::CANCELLED;
      }
      *block_msg = *encode_block(block);
    }

    return Status::OK;
	}

	Status GetHeartbeat(ServerContext* context, const Empty* empty, Empty* empty) override {
		return Status::OK;
	}
private:
  comm_thread_args* ctap_;
  Client* client_;
};

void RunServer(comm_thread_args* ctap, Client* client) {
  std::string server_address("0.0.0.0:50051");
  MinerServiceImpl service(ctap, client);

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
