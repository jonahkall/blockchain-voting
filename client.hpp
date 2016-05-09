#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"
#include "encoding_helpers.hpp"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using onevote::Empty;
using onevote::BlockMsg;
using onevote::TransactionMsg;
using onevote::AddrRequest;
using onevote::AddrResponse;
using onevote::TransactionRequest;
using onevote::BlockRequest;

class Client {
public:
  Client(std::string firstAddr);
  void BroadcastBlock(block* block);
  void BroadcastTransaction(transaction* transaction);
  int checkHeartbeats();
  std::list<std::string*> getPeerList();

private:
  std::list<SinglePeerClient*> peer_clients_;
  void addNewPeer(std::string addr);
  bool successHearbeat(const SinglePeerClient*& peer_client);
};

class SinglePeerClient {
  public:
    SinglePeerClient(std::shared_ptr<Channel> channel, std::string addr);
    Status BroadcastBlock(block* block);
    Status BroadcastTransaction(transaction* transaction);
    bool GetHeartbeat();

 private:
  std::string addr_;
  std::unique_ptr<Miner::Stub> stub_;
};
