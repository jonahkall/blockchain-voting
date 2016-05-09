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
    peer_client->BroadcastTransaction(block);
  }
}

void Client::checkHeartbeats() {
  peer_clients_.remove_if(successHearbeat);
  return peer_clients_.size();
}

std::list<std::string*>* Client::getPeersList() {
  std::list<std::string*>* peer_list = new std::list<std::string*>();
  for (const auto& peer_client: peer_clients_) {
    peer_list->push(peer_client->getAddr());
  }
  return peer_list;
}

void Client::addNewPeer(std::string addr) {
  SinglePeerClient* peer_client = new SinglePeerClient(
    grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()), 
    addr);
  peer_clients_->push(peer_client);
}

void getNewPeers(synchronized_queue<std::string*>* peerq) {
  for (const auto& peer_client: peer_clients_i) {
    AddrResponse response = peer_client->GetAddr();
    if (!response) {
      return
    }
    while (addr = response->get_peer()) {
      peerq->push(addr)
    }
  }
}

bool Client::successHearbeat(const SinglePeerClient*& peer_client) { 
  return peer_client->GetHeartbeat() 
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
    std::string prefix("Broadcast ");
  } else {
    std::string prefix("Failed to broadcast ");
  }

  std::cout << prefix << "Block " << block->block_number << " to " << addr_ << std::endl;

  return status;
}

Status SinglePeerClient::BroadcastTransaction(transaction* transaction) {
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

bool SinglePeerClient::GetHeartbeat() {
  Empty empty_req;
  Empty empty_resp;
  ClientContext context;
  Status status = stub_->GetHeartbeat(&context, empty_req, &empty_resp);
  return status.ok();
}

AddrResponse GetAddr() {
  AddrRequest addr_req;
  addr_req->set_num_requested(2);
  AddrResponse* addr_resp = new AddrResponse;
  ClientContext context;
  Status status = stub_->GetAddr(&context, addr_req, addr_resp);
  if (status.ok()) {
    return addr_resp;
  } else {
    return NULL;
  }
}
