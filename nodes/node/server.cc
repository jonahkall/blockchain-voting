include <iostream>
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

// TODO THESE NEED TO Block and Transaction rather than BlockMsg and TransactionMsg
std::queue<Block> blocks;
std::queue<Transaction> transactions;

// Wish list: 
// 	Blockchain function to get block #
// 	Function to get transaction from Blockchain
// 	Function to get peers in the main file

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
