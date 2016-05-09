#include "server.hpp"

MinerServiceImpl::MinerServiceImpl(comm_thread_args* ctap, Client* client) : Miner::Service() {
  client_ = client;
  ctap_ = ctap;
}

Status MinerServiceImpl::BroadcastBlock(ServerContext* context, const BlockMsg* block_msg, Empty* empty) {
  ctap_->bq->push(decode_block(block_msg));
	return Status::OK;
}

Status MinerServiceImpl::BroadcastTransaction(ServerContext* context, const TransactionMsg* transaction_msg, Empty* empty) {
  ctap_->tq->push(decode_transaction(transaction_msg));
	return Status::OK;
}

Status MinerServiceImpl::GetAddr(ServerContext* context, const AddrRequest* addr_req, AddrResponse* addr_resp)  {
  for (const auto& peer: *client_->getPeersList()) {
    addr_resp->add_peer(*peer);
  }
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
