#include "client.hpp"

#define MAX_PEERS 10
#define MAX_SECOND_DEGREE_FROM_PEER 2

Client::Client(std::string my_address, std::string first_peer) {
  my_address_ = my_address;
  first_peer_ = first_peer;
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
  // TODO don't add new peer if it's already in it.

  if (peer_clients_.size() >= MAX_PEERS) {
    return;
  } else if (addr == my_address_) {
    return;
  }

  clientLog("Adding peer: " + addr);
  SinglePeerClient* peer_client = new SinglePeerClient(
    grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()),
    my_address_,
    addr);
  peer_clients_.push_front(peer_client);
};

bool Client::successHearbeat(const SinglePeerClient*& peer_client) { 
  return true;
  // return peer_client->GetHeartbeat();
};

int Client::bootstrapPeers() {
  addNewPeer(first_peer_);

  std::list<std::string> new_peers;
  for (const auto& peer_client: peer_clients_) {
    AddrResponse response = peer_client->GetAddr();
    for (int i = 0; i < response.peer_size(); ++i) {
      new_peers.push_front(response.peer(i));
    }
  }
  for (const auto& new_peer: new_peers) {
    if (peer_clients_.size() >= MAX_PEERS) {
      break;
    }
    addNewPeer(new_peer);
  }
  clientLog("Number of peers: " + std::to_string(peer_clients_.size()));
  return peer_clients_.size();
};

void Client::clientLog(std::string message) {
  std::cout << "Client log: " << message << std::endl;
}

SinglePeerClient::SinglePeerClient(std::shared_ptr<Channel> channel, std::string my_addr, std::string addr)
    : stub_(Miner::NewStub(channel)) {
      addr_ = addr;
      my_addr_ = my_addr;
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
  context.AddMetadata("address", my_address_);
  AddrRequest req;
  req.set_num_requested(MAX_SECOND_DEGREE_FROM_PEER);
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
