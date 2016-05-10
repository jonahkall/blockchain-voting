#include "encoding_helpers.hpp"

#define PUBLIC_KEY_SIZE 20

/******************************************
 * Helper functions to translate objects  *
 * to and from the structures that can be *
 * passed via protocol buffers.           *
 ******************************************/

// Transactions
// ------------

// Allocates a transaction object and populates it with
// information parsed from the TransactionMsg obtained
// from the protocol buffer. Returns a pointer to the
// transaction.
transaction* decode_transaction(const TransactionMsg* transaction_msg) {
  transaction* decoded_transaction = new transaction;

  decoded_transaction->sender_public_key = transaction_msg->sender_public_key();
  decoded_transaction->vote              = transaction_msg->vote();
  decoded_transaction->timestamp         = transaction_msg->timestamp();

  return decoded_transaction;
}

// Allocates a TransactionMsg object and populates it with
// the transaction information (to be sent via protocol
// buffer). Returns a pointer to the TransactionMsg.
TransactionMsg* encode_transaction(const transaction* transaction) {
  TransactionMsg* encoded_transaction = new TransactionMsg;

  encoded_transaction->set_sender_public_key(transaction->sender_public_key);
  encoded_transaction->set_vote(transaction->vote);
  encoded_transaction->set_timestamp(transaction->timestamp);

  return encoded_transaction;
}

// Blocks
// ------

// Analogous to decode_transaction.
block* decode_block(const BlockMsg* block_msg) {
  // Allocate a new block (to be parsed from the BlockMsg)
  block* decoded_block = new block;

  // Parse from the required elements
  decoded_block->block_number        = block_msg->block_number();
  decoded_block->prev_block_SHA1     = (char*) block_msg->prev_block_sha1().c_str();
  decoded_block->magic               = block_msg->magic();
  decoded_block->merkle_root         = (char*) block_msg->merkle_root().c_str();
  decoded_block->max_ind             = block_msg->num_transactions();
  decoded_block->verifier_public_key = (char*) block_msg->verifier_public_key().c_str();
  decoded_block->finhash             = (char*) block_msg->final_hash().c_str();

  // Populate the transaction array with pointers to decoded transactions
  for (int i = 0; i < block_msg->transaction_msg_size(); i++) {
    decoded_block->transaction_array[i] = decode_transaction(&block_msg->transaction_msg(i));
  }

  // Return a pointer to the parsed block
  return decoded_block;
}

// Analogous to encode_transaction.
BlockMsg* encode_block(const block* block) {
  // Allocate a new BlockMsg (to be passed via protocol buffers)
  BlockMsg* encoded_block = new BlockMsg;

  // Populate the required elements
  encoded_block->set_block_number(block->block_number);
  encoded_block->set_prev_block_sha1(block->prev_block_SHA1, PUBLIC_KEY_SIZE);
  encoded_block->set_magic(block->magic);
  encoded_block->set_merkle_root(block->merkle_root, PUBLIC_KEY_SIZE);
  encoded_block->set_num_transactions(block->max_ind);
  encoded_block->set_verifier_public_key(block->verifier_public_key);
  encoded_block->set_final_hash(block->finhash, PUBLIC_KEY_SIZE);

  // Populate the repeated element (transactions)
  for (int i = 0; i < block->max_ind; i++) {
    TransactionMsg* transaction_msg = encoded_block->add_transaction_msg();

    transaction_msg->set_sender_public_key(block->transaction_array[i]->sender_public_key);
    transaction_msg->set_vote(block->transaction_array[i]->vote);
    transaction_msg->set_timestamp(block->transaction_array[i]->timestamp);
  }

  // Return a pointer to the BlockMsg
  return encoded_block;
}
