#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include <list>
#include <chrono>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <unordered_set>
#include <openssl/sha.h>
#include <string.h>

#define NUM_TRANSACTIONS_PER_BLOCK 64

// This is the number of times to try hashing before
// checking for a new block or a new transaction
#define NUM_MAGIC_TO_TRY 2048

// This is the number of leading zeros in hex 
#define NUM_LEADING_ZEROS 10

struct transaction {
  size_t size;
  std::string sender_public_key;
  std::string vote; // Public key of person you are voting for.
  double timestamp;
};

enum block_validity_code {
	OK,
	PREV_BLOCK_NONMATCH,
	TRANSACTION_INVALID
};

// Block (SHA: Block Number, Pointer to previous block, Magic String, Merkel Root,
// List of transactions, Identity of verifying organization (public key?)
class block {
  public:
    unsigned block_number;
    char* prev_block_SHA1;
    unsigned long long magic;
    char* merkle_root;
    transaction* transaction_array[NUM_TRANSACTIONS_PER_BLOCK];
    int max_ind;
    char* verifier_public_key;
    char* finhash;

  block();
  unsigned char* calculate_finhash();
  unsigned char* calculate_merkle_root();
  char* verify_block_number();
  
};

typedef std::list<block*> BlockList;

// TODO: make a constructor, and initialize the blockchain to have a standardized start block.
class blockchain {
  public:
    bool verify_transactions(block* b);
    bool verify_transactions();
    block_validity_code check_block_validity(block* b);
    bool add_block(block* b);
    block* get_head_block();
    int chain_length;
    void repair_blockchain(block* b);
    BlockList::iterator check_if_block_in_chain(block* b);
    void remove_transactions_from_set(block* b);
    void add_transactions_to_set(block* b);
    void add_transactions_to_queue(block* b);
    blockchain(); // Constructor

  private:
  	BlockList blocks_;

  public:
  	// A set containing the public keys of 
  	// O(1)
  	std::unordered_set<std::string> voted;
};

template <class T>
class synchronized_queue {
  private:
  	std::queue<T> queue_;
  	pthread_mutex_t lock_;
  	pthread_cond_t cv_;
  
  public:
  	void init();
  	void push(T t);
  	T pop();
  	T pop_nonblocking();
  	bool empty();
};


#endif