#include "server.hpp"

MinerServiceImpl::MinerServiceImpl(comm_thread_args* ctap, Client* client) : Miner::Service() {
  client_ = client;
  ctap_ = ctap;
}

/*
  gRPC method: Decodes a block from the message and then pushes it onto the queue. 
*/
Status MinerServiceImpl::BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) {
  block* block = decode_block(block_msg);

  // the queue is synchronized
  ctap_->bq->push(block);

  serverLog("Received block broadcast: " + std::to_string(block->block_number));
	return Status::OK;
}

/*
  gRPC method: Decodes a transaction from the message and pushes it onto the queue. 
*/
Status MinerServiceImpl::BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) {
  transaction* transaction = decode_transaction(transaction_msg);

  // the queue synchronized
  ctap_->tq->push(transaction);

  serverLog("Received transaction broadcast: " + transaction->vote);
	return Status::OK;
}

/*
  gRPC method: Returns peers list and adds the requester to our list.
*/
Status MinerServiceImpl::GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp)  {
  serverLog("GetAddr requested");

  // The client might not be ready yet by the time requests are sent, cancel request if that's true
  if (!client_) {
    return Status::CANCELLED;
  }

  // Add all of the peers to the AddrResponse
  for (const auto& peer: *client_->getPeersList()) {
    addr_resp->add_peer(*peer);
  }

  // Add the peer to our peers list, this ensures that the graph is
  // connected. The client must add the address to its metadata.
  auto it = context->client_metadata().find("address");
  client_->addNewPeer(it->second.data());

  return Status::OK;
}

// TODO not implemented. Not needed for core implementation of onevote.
Status MinerServiceImpl::GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg)  {
  return Status::CANCELLED;
}

/*
  gRPC method: returns a specific block given the hash, and returns the head block if no hash is provided.
*/
Status MinerServiceImpl::GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg)  {
  // An empty requests denotes that they want the head block
  if (block_req->block_hash().empty()) {
    *block_msg = *encode_block(ctap_->bc->get_head_block());
  } else {
    block* block = ctap_->bc->get_block(block_req->block_hash().c_str());
    // If we don't have the block, cancel the request.
    if (!block) {
      return Status::CANCELLED;
    }
    // otherwise send it back
    *block_msg = *encode_block(block);
  }

  return Status::OK;
}

/*
  gRPC method: returns a heartbeat.
*/
Status MinerServiceImpl::GetHeartbeat(ServerContext* context, const Empty* empty, Empty* dummy)  {
	return Status::OK;
}

/*
  Appends a message to the log.
*/
void MinerServiceImpl::serverLog(std::string message) {
  std::cout << "Server Log: " << message << std::endl;
}

void RunServer(comm_thread_args* ctap) {
  std::string server_address("0.0.0.0:50051");
  MinerServiceImpl service(ctap, ctap->client);

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
