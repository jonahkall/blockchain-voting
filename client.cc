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
  Client(std::string firstAddr) {
    addNewPeer(firstAddr);
  }

  void BroadcastBlock(Block* block) {
    for (const auto& peer_client: peer_clients_) {
      peer_client->BroadcastBlock(block);
    }
  }

  void BroadcastTransaction(Transaction* transaction) {
    for (const auto& peer_client: peer_clients_) {
      peer_client->BroadcastTransaction(block);
    }
  }

  int checkHeartbeats() {
    peer_clients_.remove_if(successHearbeat);
    return peer_clients_.size();
  }

  std::list<std::string*> getPeerList() {
    std::list<std::string*> peer_list;
    for (const auto& peer_client: peer_clients_) {
      peer_list->push(peer_client->getAddr());
    }
    return peer_list;
  }

private:
  std::list<SinglePeerClient*> peer_clients_;

  void addNewPeer(std::string addr) {
    SinglePeerClient* peer_client = new SinglePeerClient(
      grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()), 
      addr);
    peer_clients_->push(peer_client);
  }

  bool successHearbeat(const SinglePeerClient*& peer_client) { 
    return peer_client->GetHeartbeat() 
  };

};

class SinglePeerClient {
  private std::string addr_;

  public:
    SinglePeerClient(std::shared_ptr<Channel> channel, std::string addr)
        : stub_(Miner::NewStub(channel)) {
          addr_ = addr;
    }

  Status BroadcastBlock(Block* block) {
  	BlockMsg* block_msg = encode_block(block);
    Empty empty;
    ClientContext context;
    Status status = stub_->BroadcastBlock(&context, *block_msg, &empty);

    if (status.ok()) {
      std::string prefix("Broadcast ");
    } else {
      std::string prefix("Failed to broadcast ");
    }

    std::cout << prefix << "Block " << block->block_number << " to " << addr_ << std::endl;

    return status;
  }

  Status BroadcastTransaction(Transaction* transaction) {
    Transactionmsg* transaction_msg = encode_transaction(transaction);
    Empty empty;
    ClientContext context;
    Status status = stub_->BroadcastTransaction(&context, *transaction_msg, &empty);

    if (status.ok()) {
      std::string prefix("Broadcast ");
    } else {
      std::string prefix("Failed to broadcast ");
    }

    std::cout << prefix << "Transaction " << transaction->timestamp << " to " << addr_ << std::endl;

    return status;
  }

  bool GetHeartbeat() {
    Empty empty_req;
    Empty empty_resp;
    ClientContext context;
    Status stauts = stub_->GetHeartbeat(&context, empty_req, empty_resp);
    return status.ok();
  }

 private:
  std::unique_ptr<Miner::Stub> stub_;
};
