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

#define NUM_TRANSACTIONS_PER_BLOCK 64

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
    unsigned long long magic_string;
    char* merkle_root;
    transaction transaction_array[NUM_TRANSACTIONS_PER_BLOCK];
    char* verifier_public_key;

  char* calculate_merkle_root();
  char* verify_block_number();
  
};

class blockchain {
  public:
    bool verify_transactions(block* b);
    bool verify_transactions();
    block_validity_code check_block_validity(block* b);
    bool add_block(block* b);
    block* get_head_block();
    int chain_length;

  private:
  	std::list<block> blocks_;

  	// A set containing the public keys of 
  	// O(1)
  	std::unordered_set<std::string> voted_;
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
};


#endif