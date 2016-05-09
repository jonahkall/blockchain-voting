#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"
#include "peer.hpp"

#include "processor.hpp"
#include "encoding_helpers.hpp"

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
