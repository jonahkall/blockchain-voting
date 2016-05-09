#include "server.hpp"

MinerServiceImpl::MinerServiceImpl(comm_thread_args* ctap, Client* client) : Miner::Service() {
  client_ = client;
  ctap_ = ctap;
}

Status MinerServiceImpl::BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) {
  block* block = decode_block(block_msg);
  ctap_->bq->push(block);
  serverLog("Received block broadcast: " + std::to_string(block->block_number));
	return Status::OK;
}

Status MinerServiceImpl::BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) {
  transaction* transaction = decode_transaction(transaction_msg);
  ctap_->tq->push(transaction);
  serverLog("Received transaction broadcast: " + transaction->vote);
	return Status::OK;
}

Status MinerServiceImpl::GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp)  {
  serverLog("GetAddr requested");
  if (!client_) {
    return Status::CANCELLED;
  }
  for (const auto& peer: *client_->getPeersList()) {
    addr_resp->add_peer(*peer);
  }

  auto it = context->client_metadata().find("address");
  client_->addNewPeer(it->second.data());

  return Status::OK;
}

Status MinerServiceImpl::GetTransaction(ServerContext* context, const TransactionRequest* trans_req, TransactionMsg* transaction_msg)  {
  // TODO not going to implement this one
  return Status::CANCELLED;
}

Status MinerServiceImpl::GetBlock(ServerContext* context, const BlockRequest* block_req, BlockMsg* block_msg)  {
  if (!block_req->block_number()) {
    *block_msg = *encode_block(ctap_->bc->get_head_block());
  } else {
    block* block = ctap_->bc->get_block(block_req->block_number());
    if (!block) {
      return Status::CANCELLED;
    }
    *block_msg = *encode_block(block);
  }

  return Status::OK;
}

Status MinerServiceImpl::GetHeartbeat(ServerContext* context, const Empty* empty, Empty* dummy)  {
	return Status::OK;
}

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
