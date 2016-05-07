// We need a struct for the transaction queue

NUM_TRANSACTIONS_PER_BLOCK = 64

struct Transaction {
  size_t size;
  char* sender_public_key;
  char* vote; // Public key of person you are voting for.
  double timestamp; // Is there a C++ date/time library? Chrono. 
};

// Block (SHA: Block Number, Pointer to previous block, Magic String, Merkel Root,
// List of transactions, Identity of verifying organization (public key?)
class Block {
  public:
    int block_number;
    char* prev_block_SHA1;
    char* magic_string;
    char* merkle_root;
    Transaction[NUM_TRANSACTIONS_PER_BLOCK] transaction_array;
    char* verifier_public_key;

    char* calculate_merkle_root() {
      // Calculate and return the Merkle root. 
    }
  
  char* verify_block_number() {
   	// Asks neighbors for the previous block, and checks to make sure its block_number is
    // one less than this one's.
  }
  
};


class Blockchain {
  public:
  
  	// Check if a new block is valid to be added to this blockchain.
    bool verify_transactions(Block b) {
			// Make sure that none of the transactions in this block are already in the blockchain.
    }
  	// Overloaded function - this will just check if the current transactions in blockchain are valid.
    bool verify_transactions() {
      // Iterate over all of the blocks, and check that there are not two transactions from the same person.
    }
  	
    int check_block_validity(Block b) {
      // Make sure none of the votes had previously been made.
			bool transaction_verification = verify_transactions(b);
      if (!transaction_verification) 
        return -1;
      
      // Make sure the new block has the correct prev block.
      Block* head_block = get_head_block()
      if (b.prev_block_SHA1 != head_block->merkle_root)
        return -2;
        
      // If everything checks out, return 1.
      return 1;
    }
  
    bool add_block(Block b) {
      // Call the appropriate code to make sure all of the transactions are valid.
      int block_validity_result = check_block_validity(b)
      if (block_validity_result == 1) {
        // Add block to the blockchain
        return true;
        chain_length++;
      }
      return false;
    }
  
    Block* get_head_block(){
			// Get the head block. Don't remove it from the chain.
    }
  
  
  private:
  	std::vector<Block>; // Is this the best way to do this? Or do we wanna create our own linked list?
  	int chain_length; 
};

class TransactionQueue {
  private:
  	std::queue<Transaction> queue;
  	pthread_mutex_t lock;
  
  public:
  	void push(Transaction t) {
  	// Grab the lock
    // push the transaction
    // release the lock
  }
  
  	Transaction pop() {
   	// Grab the lock
    // pop the transaction
    // release the lock
  }
};





