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

#include "client.hpp"
class Client;

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

/**
  A template class for a thread-safe (synchronized) queue

  This template class defines an API for a queue that is thread safe. This has only the basic
  queue functions of pushing and popping, with the pop blocking until an element is added. 
  If pop_nonblocking() is used, then the queue returns NULL if nothing is in it.

  */
template <class T>
class synchronized_queue {
  private:
    // The underlying queue data structure
  	std::queue<T> queue_;
    // A mutex lock used to avoid race conditions
  	pthread_mutex_t lock_;
    // A condvar used for blocking when empty.
  	pthread_cond_t cv_;
  
  public:
    // Initialize the queue. In particular, this should initialize the locking code.
  	void init();

    /* 
      Push an element of type T onto the thread-safe queue. 

      \param t: an element of type T to be pushed on the queue.
    */
  	void push(T t);

    /* 
      Pop from the front of the queue. Block if queue is empty until an item comes in.

      \return item of type T from front of queue. 
    */
  	T pop();

    /* 
      Pop from the front of the queue. Return NULL if empty instead of blocking.

      \return item of type T from front of queue. 
    */
  	T pop_nonblocking();

    /* 
      Indicate whether queue is empty.

      \return boolean indicating whether queue is empty. 
    */
  	bool empty();
};

// A simple typedef to make notation cleaner elsewhere.
typedef std::list<block*> BlockList;

/*
  blockchain

  This class contains the blockchain code. In particular, it contains a doubly linked list with stores the actual
  blocks of the blockchain. It also contains a variety of helper functions to interface with the blockchain, including
  methods to get blocks, add them, verify their existence, and repair the blockchain when a better chain is found from 
  a peer. 
  */
class blockchain {
  public:
    /* 
      Iterate over the transactions in the block and make sure none were already in the blockchain.

      \param b, the block those transaction are being verified. 
      \return boolean, indicating whether the block's transactions are all new.
    */
    bool verify_transactions(block* b);

    // Unimplemented. Disregard.
    bool verify_transactions();

    /* 
      Checks whether the incoming block is valid, and returns the appopriate validity code.
      This will indicate whether the block is OK to be added to the chain, has invalid transactions,
      or if its previous block doesn't match the head of the current one, meaning a blockchain
      repair may be needed.

      \param b, the block being considered
      \return block_validity_code enum element indicating the result.
    */
    block_validity_code check_block_validity(block* b);

    /* 
      Add a block to the head of a blockchain.
      Assumptions: This function assumes that the incoming block is OK to add to the head. It does not
      run any additional checks.

      \param b, the block being added
      \return boolean, indicating success/failure
    */
    bool add_block(block* b);

    /*
      Get's the block currently at the head of the blockchain.

      \return block*, a pointer to hhe head block
    */
    block* get_head_block();

    // The current length of the blockchain
    int chain_length;

    /*
     Given a new block b, repairs the blockchain to match the history of b. 

     \param b, the block that is being added, and whose history determines the repair
     \return void
     */
    void repair_blockchain(block* b, Client* client);

    /*
      Searches for block b in the chain, and returns the iterator to it if so.
      Assumptions: A block can be found by just comparing the hashes for equality.
      This assumptions that two distinct blocks do not share a hash. Based on  research about
      blockchain and its encryption algorithms, this is a reasonable assumption. 

      \param b, the block being searched.
      \return BlockList::iterator to the block. Will return BlockList::end() if not found
    */
    BlockList::iterator check_if_block_in_chain(block* b);

    /*
     Remove all of the transactions from block b from the set of votes. 

     /param b, the block whose transactions are being removed.
     */
    void remove_transactions_from_set(block* b);

    /*
     Add all of the transactions from block b to the set of votes. 

     /param b, the block whose transactions are being added.
     */
    void add_transactions_to_set(block* b);

    /*
     Add all of the transactions from block b from the queue of transactinos to process. 

     /param b, the block whose transactions are being added.
     */
    void add_transactions_to_queue(block* b);

    /* 
      A constructor. 
      \param q, takes a pointer to the thread-safe queue of transactions. This is needed so that in the case
      of a block chain repair, transactions can be readded to the queue.
    */
    blockchain(synchronized_queue<transaction*>* q); // Constructor

    /*
      Return a pointer to a block based on its block number
      \param n, the block number
      \return block*, the block matching that number
    */
    block* get_block(int n);

    /*
      Return a pointer to a block based on its hash
      Assumptions: A block can be found by just comparing the hashes for equality.
      This assumptions that two distinct blocks do not share a hash. Based on  research about
      blockchain and its encryption algorithms, this is a reasonable assumption. 

      \param n, the hash
      \return block*, the block matching that hash
    */
    block* get_block(char* hash);

  private:
    // This is the doubly linked list of blocks
  	BlockList blocks_;

    // This is the reference to the shared transaction queue
  	synchronized_queue<transaction*>* q_ptr_;

  public:
  	// A set containing the public keys of the people that voted. 
    // We use an unordered set for maximum efficiency.
  	std::unordered_set<std::string> voted;
};


#endif
