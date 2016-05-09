#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"

using onevote::Empty;
using onevote::BlockMsg;
using onevote::TransactionMsg;
using onevote::AddrRequest;
using onevote::AddrResponse;
using onevote::TransactionRequest;
using onevote::BlockRequest;

transaction* decode_transaction(const TransactionMsg* transaction_msg);
TransactionMsg* encode_transaction(transaction* transaction);
block* decode_block(BlockMsg* block_msg);
BlockMsg* encode_block(block* block);
