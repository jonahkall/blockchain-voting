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

// Logic and data behind the server's behavior.
class MinerServiceImpl final : public Miner::Service {
  public:
    MinerServiceImpl(comm_thread_args* ctap, Client* client);
  	Status BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty);
  	Status BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty);
  	Status GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp);
  	Status GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg);
  	Status GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg);
  	Status GetHeartbeat(ServerContext* context, const Empty* empty, Empty* dummy);

  private:
    comm_thread_args* ctap_;
    Client* client_;
};

void RunServer(comm_thread_args* ctap, Client* client);
