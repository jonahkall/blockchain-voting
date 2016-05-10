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

block* Client::getBlock(char* block_hash) {
  assert(block_hash);
  for (const auto& peer_client: peer_clients_) {
    clientLog("Asking for block " + std::string(block_hash) + " from " + *peer_client->peerAddr());
    block* block = peer_client->GetBlock(block_hash);
    if (block) {
      clientLog("Found!");
      return block;
    }
  }
  clientLog("Failed to find block " + std::string(block_hash));
  return NULL;
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
  // validate number of peers or address is not own
  if ((peer_clients_.size() >= MAX_PEERS) || addr == my_address_ || addr == NOT_AN_IP_TOKEN) {
    return;    
  }

  // make sure the peer has not already been added
  for (const auto& peer_client: peer_clients_) {
    if (*peer_client->peerAddr() == addr) {
      return;
    }
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
  std::cout << "Client " << my_address_ << " log: " << message << std::endl;
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
    std::cout << "Broadcast block " << block->block_number << " to " << addr_ << std::endl;
  } else {
    std::cout << "Failed to broadcast " << block->block_number << " to " << addr_ << std::endl;
  }

  return status;
}

Status SinglePeerClient::BroadcastTransaction(transaction* transaction) {
  TransactionMsg* transaction_msg = encode_transaction(transaction);
  Empty empty;
  ClientContext context;
  Status status = stub_->BroadcastTransaction(&context, *transaction_msg, &empty);

  if (status.ok()) {
    std::cout << "Broadcast transaction " << transaction->vote << " to " << addr_ << std::endl;
  } else {
    std::cout << "Failed to Broadcast transaction " << transaction->vote << " to " << addr_ << std::endl;
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
  context.AddMetadata("address", my_addr_);
  AddrRequest req;
  req.set_num_requested(MAX_SECOND_DEGREE_FROM_PEER);
  AddrResponse resp;
  Status status = stub_->GetAddr(&context, req, &resp);
  return resp;
}

block* SinglePeerClient::GetBlock(char* block_hash) {
  assert(block_hash); 
  ClientContext context;
  BlockRequest req;
  BlockMsg blockmsg;
  req.set_block_hash(block_hash);
  Status status = stub_->GetBlock(&context, req, &blockmsg);
  if (!status.ok()) {
    return NULL;
  }

  return decode_block(&blockmsg);
}

bool SinglePeerClient::GetHeartbeat() {
  Empty empty_req;
  Empty empty_resp;
  ClientContext context;
  Status status = stub_->GetHeartbeat(&context, empty_req, &empty_resp);
  return status.ok();
}
