#include "processor.hpp"

using namespace std;

#define NUM_TRANSACTIONS_PER_BLOCK = 64

struct transaction {
  size_t size;
  char* sender_public_key;
  char* vote; // Public key of person you are voting for.
  double timestamp; // Is there a C++ date/time library? Chrono. 
};

// Block (SHA: Block Number, Pointer to previous block, Magic String, Merkel Root,
// List of transactions, Identity of verifying organization (public key?)
class block {
  public:
    int block_number;
    char* prev_block_SHA1;
    char* magic_string;
    char* merkle_root;
    int x[10];
    //transaction transaction_array[NUM_TRANSACTIONS_PER_BLOCK];
    char* verifier_public_key;

    char* calculate_merkle_root() {
    // Calculate and return the Merkle root. 
    	return NULL;
    }
  
  char* verify_block_number() {
   	// Asks neighbors for the previous block, and checks to make sure its block_number is
    // one less than this one's.
    return NULL;
  }
  
};


class blockchain {
  public:
  
  	// Check if a new block is valid to be added to this blockchain.
    bool verify_transactions(block b) {
			// Make sure that none of the transactions in this block are already in the blockchain.
    	return false;
    }
  	// Overloaded function - this will just check if the current transactions in blockchain are valid.
    bool verify_transactions() {
      // Iterate over all of the blocks, and check that there are not two transactions from the same person.
    	return true;
    }
  	
    int check_block_validity(block b) {
      // Make sure none of the votes had previously been made.
			bool transaction_verification = verify_transactions(b);
      if (!transaction_verification) 
        return -1;
      
      // Make sure the new block has the correct prev block.
      block* head_block = get_head_block();
      if (b.prev_block_SHA1 != head_block->merkle_root)
        return -2;
        
      // If everything checks out, return 1.
      return 1;
    }
  
    bool add_block(block b) {
      // Call the appropriate code to make sure all of the transactions are valid.
      int block_validity_result = check_block_validity(b);
      if (block_validity_result == 1) {
        // Add block to the blockchain
        chain_length_++;
        return true;
      }
      return false;
    }
  
    block* get_head_block(){
			// Get the head block. Don't remove it from the chain.
			return NULL;
    }
  
  
  private:
  	std::vector<block> blocks_; // Is this the best way to do this? Or do we wanna create our own linked list?
  	int chain_length_; 
};

class transaction_queue {
  private:
  	std::queue<transaction> queue_;
  	pthread_mutex_t lock_;
  
  public:
  	void init() {

  	}

  	void push(transaction t) {

  	}
  
  	transaction pop() {
  		return transaction();
  	}
};





