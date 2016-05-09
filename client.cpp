#include "client.hpp"

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
  return peer_clients_.size();
}

std::list<std::string*>* Client::getPeersList() {
  std::list<std::string*>* peer_list = new std::list<std::string*>();
  for (const auto& peer_client: peer_clients_) {
    peer_list->push_front(peer_client->peerAddr());
  }
  return peer_list;
}

void Client::addNewPeer(std::string addr) {
  SinglePeerClient* peer_client = new SinglePeerClient(
    grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()), 
    addr);
  peer_clients_.push_front(peer_client);
}

bool Client::successHearbeat(const SinglePeerClient*& peer_client) { 
  return true;
  //return peer_client->GetHeartbeat();
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

bool SinglePeerClient::GetHeartbeat() {
  Empty empty_req;
  Empty empty_resp;
  ClientContext context;
  Status status = stub_->GetHeartbeat(&context, empty_req, &empty_resp);
  return status.ok();
}
