#include "encoding_helpers.hpp"

transaction* decode_transaction(const TransactionMsg* transaction_msg) {
  transaction* decoded_transaction = new transaction;

  decoded_transaction->sender_public_key = transaction_msg->sender_public_key();
  decoded_transaction->vote              = transaction_msg->vote();
  decoded_transaction->timestamp         = transaction_msg->timestamp();

  return decoded_transaction;
}

TransactionMsg* encode_transaction(const transaction* transaction) {
  TransactionMsg* encoded_transaction = new TransactionMsg;

  encoded_transaction->set_sender_public_key(transaction->sender_public_key);
  encoded_transaction->set_vote(transaction->vote);
  encoded_transaction->set_timestamp(transaction->timestamp);

  return encoded_transaction;
}

block* decode_block(const BlockMsg* block_msg) {
  block* decoded_block = new block;

  decoded_block->block_number        = block_msg->block_number();
  decoded_block->prev_block_SHA1     = (char*) block_msg->prev_block_sha1().c_str();
  decoded_block->magic               = block_msg->magic();
  decoded_block->merkle_root         = (char*) block_msg->merkle_root().c_str();
  decoded_block->max_ind             = block_msg->num_transactions();
  decoded_block->verifier_public_key = (char*) block_msg->verifier_public_key().c_str();
  decoded_block->finhash             = (char*) block_msg->final_hash().c_str();

  for (int i = 0; i < block_msg->transaction_msg_size(); i++) {
    decoded_block->transaction_array[i] = decode_transaction(&block_msg->transaction_msg(i));
  }

  return decoded_block;
}

BlockMsg* encode_block(const block* block) {
  BlockMsg* encoded_block = new BlockMsg;

  encoded_block->set_block_number(block->block_number);
  encoded_block->set_prev_block_sha1(block->prev_block_SHA1);
  encoded_block->set_magic(block->magic);
  encoded_block->set_merkle_root(block->merkle_root);
  encoded_block->set_num_transactions(block->max_ind);
  encoded_block->set_verifier_public_key(block->verifier_public_key);
  encoded_block->set_final_hash(block->finhash);

  for (int i = 0; i < block->max_ind; i++) {
    TransactionMsg* transaction_msg = encoded_block->add_transaction_msg();

    transaction_msg->set_sender_public_key(block->transaction_array[i]->sender_public_key);
    transaction_msg->set_vote(block->transaction_array[i]->vote);
    transaction_msg->set_timestamp(block->transaction_array[i]->timestamp);
  }

  return encoded_block;
}
