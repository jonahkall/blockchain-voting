#include "client.hpp"

#define MAX_PEERS 10
#define MAX_SECOND_DEGREE_FROM_PEER 2
#define PUBLIC_KEY_SIZE 20

Client::Client(std::string my_address, std::string first_peer) {
  my_address_ = my_address;
  first_peer_ = first_peer;
}

// Sends a block to all of the peers in its network
// \param block is a block that is either created or needs to be rebroadcast, it
// should only do this if the block is deemed valid by the server
void Client::BroadcastBlock(block* block) {
  for (const auto& peer_client: peer_clients_) {
    peer_client->BroadcastBlock(block);
  }
}

// Sends a transaction to all of the peers in its network
// \param transaction is a transaction that is created or needs to be rebroadcast
void Client::BroadcastTransaction(transaction* transaction) {
  for (const auto& peer_client: peer_clients_) {
    peer_client->BroadcastTransaction(transaction);
  }
}

// Asks for a block with a specific hash from each of its peers
// if none is found, returns NULL
// \param block_hash is a hash of the block we're looking for, it is of length
// PUBLIC_KEY_SIZE
// \return a reference to a block or NULL if nothing was found
block* Client::getBlock(char* block_hash) {
  for (const auto& peer_client: peer_clients_) {
    clientLog("Asking for block from " + *peer_client->peerAddr());
    block* block = peer_client->GetBlock(block_hash);
    if (block) {
      clientLog("Found!");
      return block;
    }
  }
  return NULL;
}

/*
  Not implemented
*/
int Client::checkHeartbeats() {
  return 0;
}

/*
  Gets the list of actively connected peers and returns a copy as
  a new list of strings which are mutable.
*/
std::list<std::string*>* Client::getPeersList() {
  std::list<std::string*>* peer_list = new std::list<std::string*>();
  for (const auto& peer_client: peer_clients_) {
    peer_list->push_front(peer_client->peerAddr());
  }
  return peer_list;
};

/*
  Adds a new peer to the broadcast network and ensures invariants
  1) Only MAX_PEERS number of peers
  2) None of the peers is itself
  3) None of the peers are duplicated
  4) The peers have a legitimate IP address and port.
*/
void Client::addNewPeer(std::string addr) {
  // validate number of peers, address is not own, and it's not a dummy address
  if ((peer_clients_.size() >= MAX_PEERS) || addr == my_address_ || addr == NOT_AN_IP_TOKEN) {
    return;    
  }

  // make sure the peer has not already been added. O(n) algorithm.
  for (const auto& peer_client: peer_clients_) {
    if (*peer_client->peerAddr() == addr) {
      return;
    }
  }

  clientLog("Adding peer: " + addr);
  // establishes a SinglePeer Client and pushes it to the list of peer_clients.
  SinglePeerClient* peer_client = new SinglePeerClient(
    grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()),
    my_address_,
    addr);
  peer_clients_.push_front(peer_client);
};

/*
  Not implemented.
*/
bool Client::successHearbeat(const SinglePeerClient*& peer_client) { 
  return true;
};

/*
  Refills its peers queue by asking all of the current peers for their peers
  and then adds them to the queue.
*/
int Client::bootstrapPeers() {
  // checks to see if the first_peer_ is already added
  addNewPeer(first_peer_);

  // gets the new_peers from all of the peers that are already connected
  std::list<std::string> new_peers;
  for (const auto& peer_client: peer_clients_) {
    AddrResponse response = peer_client->GetAddr();
    for (int i = 0; i < response.peer_size(); ++i) {
      new_peers.push_front(response.peer(i));
    }
  }

  // adds all of the new_peers onto the peers list
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

/*
  gRPC method: Broadcasts a block to the peer
*/
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

/*
  gRPC method: Broadcasts a transaction to the peer
*/
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

/*
  Returns the address of the peer that this SinglePeerClient is working with
  makes a copy as a string.
*/
std::string* SinglePeerClient::peerAddr() {
  std::string* ret = new std::string;
  *ret = addr_;
  return ret;
}

/*
  gRPC method: Gets all of the peers of this current peer
  Returns it as an AddrResponse that can be called.
*/
AddrResponse SinglePeerClient::GetAddr() {
  ClientContext context;

  // needs to send our address so that they can 
  // add us to their peer network too
  context.AddMetadata("address", my_addr_);
  AddrRequest req;
  
  // Sets a limit on the number of peers that we need
  req.set_num_requested(MAX_SECOND_DEGREE_FROM_PEER);
  AddrResponse resp;
  Status status = stub_->GetAddr(&context, req, &resp);
  return resp;
}

/*
  gRPC method: Gets a specific block from the peer.
*/
block* SinglePeerClient::GetBlock(char* block_hash) {
  std::cout << "Looking for block " << block_hash;
  ClientContext context;
  BlockRequest req;
  BlockMsg blockmsg;
  // Must copy the block_hash over by PUBLIC_KEY_SIZE amount
  // otherwise copying a string will fail as block_hash can have
  // 0's.
  req.set_block_hash(block_hash, PUBLIC_KEY_SIZE);
  Status status = stub_->GetBlock(&context, req, &blockmsg);
  if (!status.ok()) {
    return NULL;
  }

  return decode_block(&blockmsg);
}

/*
  gRPC method: Gets a heartbeat from the peer and 
  returns true if it's still alive.
*/
bool SinglePeerClient::GetHeartbeat() {
  Empty empty_req;
  Empty empty_resp;
  ClientContext context;
  Status status = stub_->GetHeartbeat(&context, empty_req, &empty_resp);
  return status.ok();
}
