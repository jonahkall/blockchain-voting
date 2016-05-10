#ifndef __SERVER_H__
#define __SERVER_H__

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"
#include "peer.hpp"
struct comm_thread_args;

#include "processor.hpp"
#include "encoding_helpers.hpp"
#include "client.hpp"

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

/*
  Implements a gRPC service for the Miner. It will be run 
  on a seperate thread with RunServer.
*/
class MinerServiceImpl final : public Miner::Service {
  public:
    // Initializes a MinerServiceImpl using the arguments passed to the 
    // communications thread and needs a client.
    MinerServiceImpl(comm_thread_args* ctap, Client* client);

    // Accepts a block and appends it to the block queue for processor
  	Status BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) override;

    // Accepts a transaction and appends it to the transaction queue for processor
  	Status BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) override;

    // Returns the peers that this machine is connected to and, if space is available, adds the
    // sender to the list of peers
  	Status GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp) override;

    // Returns a transaction in the blockchain
  	Status GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg) override;

    // Returns a specific block in the blockchain
  	Status GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg) override;

    // Returns a heartbeat
  	Status GetHeartbeat(ServerContext* context, const Empty* empty, Empty* dummy) override;

  private:
    comm_thread_args* ctap_;
    Client* client_;
    void serverLog(std::string message);
};

/*
  Runs the MinerServiceImpl, should be called in a seperate communications thread.
  It will wait until the server thread is killed,
*/
void RunServer(comm_thread_args* ctap);

#endif
