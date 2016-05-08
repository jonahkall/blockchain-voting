#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using onevote::EmptyRequest;
using onevote::Success;
using onevote::BlockMsg;
using onevote::TransactionMsg;
using onevote::AddrRequest;
using onevote::AddrResponse;
using onevote::TransactionRequest;
using onevote::BlockRequest;

std::queue<std::string> peers;

std::queue<Block> blocks;
std::queue<Transaction> transactions;

Block* decode_block(BlockMsg* block_msg) {
  Block* decoded_block = new Block;

  decoded_block->block_number        = block_msg->block_number();
  decoded_block->prev_block_SHA1     = block_msg->prev_block_sha1();
  decoded_block->magic               = block_msg->magic();
  decoded_block->merkle_root         = block_msg->merkle_root();
  decoded_block->max_ind             = block_msg->num_transactions();
  decoded_block->verifier_public_key = block_msg->verifier_public_key();
  decoded_block->finhash             = block_msg->final_hash();
  decoded_block->transaction_array   = new Transaction[block_msg->transaction_msg_size()]

  for (int i = 0; i < block_msg->transaction_msg_size(); i++) {
    decoded_block->transaction_array[i] = decode_transaction(block_msg->transaction_msg(i));
  }

  return decoded_block;
}

BlockMsg* encode_block(Block* block) {
  BlockMsg* encoded_block = new BlockMsg;

  encoded_block->set_block_number(block->block_number);
  encoded_block->set_prev_block_sha1(block->prev_block_SHA1);
  encoded_block->set_magic(block->magic);
  encoded_block->set_merkle_root(block->merkle_root);
  encoded_block->set_num_transactions(block->max_ind);
  encoded_block->set_verifier_public_key(block->verifier_public_key);
  encoded_block->set_final_hash(block->finhash);

  for (int i = 0; i < block->max_ind; i++) {
    encoded_block->add_transaction_msg(*encode_transaction(*block->transaction_array[i]))
  }

  return encoded_block;
}

Transaction* decode_transaction(TransactionMsg* transaction_msg) {
  Transaction* decoded_transaction = new Transaction;

  decoded_transaction->sender_public_key = transaction_msg->sender_public_key();
  decoded_transaction->vote              = transaction_msg->vote();
  decoded_transaction->timestamp         = transaction_msg->timestamp();

  return decoded_transaction;
}

TransactionMsg* encode_transaction(Transaction* transaction) {
  TransactionMsg* encoded_transaction = new TransactionMsg;

  encoded_transaction->set_sender_public_key(transaction->sender_public_key);
  encoded_transaction->set_vote(transaction->vote);
  encoded_transaction->set_timestamp(transaction->timestamp);

  return encoded_transaction;
}

// Logic and data behind the server's behavior.
class MinerServiceImpl final : public Miner::Service {
	Status BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) override {
		// turn blockmsg into block
		blocks.push_back(block_msg);
		return Status::OK;
	}

	Status BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) override {
		// turn transactionmsg into transaction
		transactions.push_back(transaction_msg);
		return Status::OK;
	}

	Status GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp) override {

	}

	Status GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg) override {

	}

	Status GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg) override {

	}

	Status GetHeartbeat(ServerContext* context, const Empty* empty, Empty* empty) override {
		return Status::OK;
	}
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

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
