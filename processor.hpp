#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>
#include <pthread.h>
#include <queue>
#include <unistd.h>
#include <list>
#include <chrono>
#include <algorithm>
#include <thread>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <unordered_set>

#define NUM_TRANSACTIONS_PER_BLOCK 64

struct transaction {
  size_t size;
  char* sender_public_key;
  char* vote; // Public key of person you are voting for.
  double timestamp;
};

// Block (SHA: Block Number, Pointer to previous block, Magic String, Merkel Root,
// List of transactions, Identity of verifying organization (public key?)
class block {
  public:
    unsigned block_number;
    char* prev_block_SHA1;
    char* magic_string;
    char* merkle_root;
    transaction transaction_array[NUM_TRANSACTIONS_PER_BLOCK];
    char* verifier_public_key;

  char* calculate_merkle_root();
  char* verify_block_number();
  
};

class blockchain {
  public:
    bool verify_transactions(block b);
    bool verify_transactions();
    int check_block_validity(block b);
    bool add_block(block b);
    block* get_head_block();
  
  private:
  	std::list<block> blocks_;
  	int chain_length_; 
};

template <class T>
class synchronized_queue {
  private:
  	std::queue<T*> queue_;
  	pthread_mutex_t lock_;
  	pthread_cond_t cv_;
  
  public:
  	void init();
  	void push(T* t);
  	T* pop();
};


#endif