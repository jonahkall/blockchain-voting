#include "client.hpp"

#define MAX_PEERS 10
#define MAX_SECOND_DEGREE_FROM_PEER 2

Client::Client(std::string firstAddr) {
  addNewPeer(firstAddr);
}

void Client::BroadcastBlock(block* block) {
  for (const auto& peer_client: peer_clients_) {
    peer_client->BroadcastBlock(block);
  }
}

void Client::BroadcastTransaction(transaction* transaction) {
  for (const auto& peer_client: peer_clients_) {
    peer_client->BroadcastTransaction(transaction);
  }
}

int Client::checkHeartbeats() {
  // peer_clients_.remove_if(successHearbeat);
  // return peer_clients_.size();
  return 0;
}

std::list<std::string*>* Client::getPeersList() {
  std::list<std::string*>* peer_list = new std::list<std::string*>();
  for (const auto& peer_client: peer_clients_) {
    peer_list->push_front(peer_client->peerAddr());
  }
  return peer_list;
};

void Client::addNewPeer(std::string addr) {
  if (peer_clients_.size() >= MAX_PEERS) {
    return;
  }

  SinglePeerClient* peer_client = new SinglePeerClient(
    grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()), 
    addr);
  peer_clients_.push_front(peer_client);
};

bool Client::successHearbeat(const SinglePeerClient*& peer_client) { 
  return true;
  // return peer_client->GetHeartbeat();
};

int Client::bootstrapPeers() {
  std::list<std::string> new_peers;
  for (const auto& peer_client: peer_clients_) {
    AddrResponse response = peer_client->GetAddr();
    std::string peer;
    while(peer = response->get_peer()) {
      new_peers.push_front(peer);
    }
  }
  for (const auto& new_peer: new_peers) {
    if (peer_clients_.size() >= MAX_PEERS) {
      break;
    }
    addNewPeer(new_peer);
  }
  return peer_clients_.size();
};

SinglePeerClient::SinglePeerClient(std::shared_ptr<Channel> channel, std::string addr)
    : stub_(Miner::NewStub(channel)) {
      addr_ = addr;
}

Status SinglePeerClient::BroadcastBlock(block* block) {
	BlockMsg* block_msg = encode_block(block);
  Empty empty;
  ClientContext context;
  Status status = stub_->BroadcastBlock(&context, *block_msg, &empty);

  if (status.ok()) {
    std::cout << "Broadcast " << "Block " << block->block_number << " to " << addr_ << std::endl;
  } else {
    std::cout << "Failed to broadcast " << "Block " << block->block_number << " to " << addr_ << std::endl;
  }

  return status;
}

Status SinglePeerClient::BroadcastTransaction(transaction* transaction) {
  TransactionMsg* transaction_msg = encode_transaction(transaction);
  Empty empty;
  ClientContext context;
  Status status = stub_->BroadcastTransaction(&context, *transaction_msg, &empty);

  if (status.ok()) {
    std::cout << "Broadcast Transaction " << transaction->timestamp << " to " << addr_ << std::endl;
  } else {
    std::cout << "Failed to Broadcast Transaction " << transaction->timestamp << " to " << addr_ << std::endl;
  }

  return status;
}

std::string* SinglePeerClient::peerAddr() {
  std::string* ret = new std::string;
  *ret = addr_;
  return ret;
}

AddrResponse SinglePeerClient::GetAddr() {
  ClientContext context;
  AddrRequest req;
  req->set_num_requested(MAX_SECOND_DEGREE_FROM_PEER);
  AddrResponse resp;
  Status status = stub_->GetAddr(&context, req, &resp);
  return resp;
}

bool SinglePeerClient::GetHeartbeat() {
  Empty empty_req;
  Empty empty_resp;
  ClientContext context;
  Status status = stub_->GetHeartbeat(&context, empty_req, &empty_resp);
  return status.ok();
}
