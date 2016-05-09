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
#include <math.h>

#define NUM_TRANSACTIONS_PER_BLOCK 64

// This is the number of times to try hashing before
// checking for a new block or a new transaction
#define NUM_MAGIC_TO_TRY 2048

// This is the number of leading zeros in hex 
#define NUM_LEADING_ZEROS 10

#define PUBLIC_KEY_SIZE 20


/**
   transaction

   A class for encapsulating all of the informatio needed for a single transaction, or vote. 
   Many of these transactions will make up a vote.
 */
struct transaction {
  size_t size;
  std::string sender_public_key;
  std::string vote; // Public key of person you are voting for.
  double timestamp;
};

/**
  This enum will contain the possible values for a block's validity.
 */
enum block_validity_code {
	OK,
	PREV_BLOCK_NONMATCH,
	TRANSACTION_INVALID
};


/**
  Block

  This a class for blocks of transactions. This class contains all of the important information for a 
  discovered or working block. In particular, it contains an array of transaction objects. Furthermore,
  it contains information about the individual that discovered the block, and block metadata, such as the has
  and magic string.
 */
class block {
  public:
    // The block number. This indicates where in the chain this block is.
    unsigned block_number;

    // This is the SHA1 hash of the previous block. It allows for the implicit linking
    // of blocks to form a blockchain
    char* prev_block_SHA1;

    // This is the magic string that is appended to the merkle root prior to getting a SHA1 hash for the whole
    // block.
    unsigned long long magic;

    // The merkle root is a hash of the transactions. 
    char* merkle_root;

    // An array of transactions in this block.
    transaction* transaction_array[NUM_TRANSACTIONS_PER_BLOCK];

    // This is used to determine the number of transactions in the block.
    int max_ind;

    // This is the public key of the individual that found this block.
    char* verifier_public_key;

    // This is the SHA1 hash of the block after combining the merkle root with the magic key.
    char* finhash;

    // A constructor for the blocl. 
    /*
      This instantiates the block and initializes the various attributes
    */
    block();
    
    /*
      Calculates the overall SHA1 hash for the block by appending the merkle root to he
      magic string and taking the SHA1 hash of the result.
    */
    unsigned char* calculate_finhash();

    /*
      Calculate the merkle root of the transactions. This requires hashing pairs of transactions in a binary tree-style format. 
    */
    unsigned char* calculate_merkle_root();

    /*
     This is uninplemented.
     */
    char* verify_block_number();
  
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


typedef std::list<block*> BlockList;

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
    blockchain(synchronized_queue<transaction*>* q); // Constructor
    block* get_block(int n);
    block* get_block(char* hash);

  private:
  	BlockList blocks_;
  	synchronized_queue<transaction*>* q_ptr_;

  public:
  	// A set containing the public keys of 
  	// O(1)
  	std::unordered_set<std::string> voted;
};


#endif