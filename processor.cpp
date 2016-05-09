#include "processor.hpp"


using namespace std;
using namespace std::chrono;

////////////////////////////////
///// block implementation /////
////////////////////////////////

// A constructor for the blocl. 
/*
  This instantiates the block and initializes the various attributes
*/
block::block() {
	prev_block_SHA1 = NULL;
	magic = 0;
	merkle_root = NULL;
	max_ind = 0;
	verifier_public_key = NULL;
	finhash = NULL;
}

/*
	Calculate the merkle root of the transactions.
	This requires hashing pairs of transactions in a binary tree-style format. 
*/
unsigned char* block::calculate_merkle_root() {

	vector<unsigned char*> old_hashes;
	vector<unsigned char*> new_hashes;
	void *naked = (void *) transaction_array;
	for (int i = 0; i < NUM_TRANSACTIONS_PER_BLOCK; ++i) {
		unsigned char* hash1 = new unsigned char[SHA_DIGEST_LENGTH];
		unsigned char* data_to_hash =
				(unsigned char*) transaction_array[0]->sender_public_key.c_str();
		SHA1(data_to_hash, PUBLIC_KEY_SIZE, hash1);
		old_hashes.push_back(hash1);
	}

	// Until we reach root, construct each level in turn.
	while (1) {
		unsigned char* buffer = new unsigned char[SHA_DIGEST_LENGTH * 2];
		for (int i = 0; i < old_hashes.size(); i += 2) {
			unsigned char* hash2 = new unsigned char[SHA_DIGEST_LENGTH];

			// Concatenate leaves into a single buffer and SHA-1 it.
			memcpy(buffer, old_hashes[i], SHA_DIGEST_LENGTH);
			memcpy(buffer + SHA_DIGEST_LENGTH, old_hashes[i+1], SHA_DIGEST_LENGTH);
			SHA1(buffer, 2 * SHA_DIGEST_LENGTH, hash2);
			new_hashes.push_back(hash2);
		}
		if (new_hashes.size() == 1) {
			break;
		}
		old_hashes = new_hashes;
		new_hashes.clear();
	}
	if (new_hashes[0] == NULL)
		assert(false);
 	return new_hashes[0];
}

// Unimplemented
char* block::verify_block_number() {
 // Asks neighbors for the previous block, and checks to make sure its 
 // block_number is one less than this one's.
  return NULL;
}

/*
  Calculates the overall SHA1 hash for the block by appending the merkle root to he
  magic string and taking the SHA1 hash of the result.
*/
void block::calculate_finhash() {
	unsigned char* hash = new unsigned char[SHA_DIGEST_LENGTH];
	const unsigned char* data_to_hash = this->calculate_merkle_root();
	unsigned char* buffer =
			new unsigned char[SHA_DIGEST_LENGTH + sizeof(unsigned long long)];
	memcpy(buffer, data_to_hash, SHA_DIGEST_LENGTH);
	memcpy(buffer + SHA_DIGEST_LENGTH, &this->magic, sizeof(unsigned long long));
	SHA1(buffer, SHA_DIGEST_LENGTH + sizeof(unsigned long long), hash);
	if (hash == NULL)
		assert(false);
	finhash = (char*)hash;
}


/////////////////////////////////////
///// blockchain implementation /////
/////////////////////////////////////

/* 
  A constructor. 
  \param q, takes a pointer to the thread-safe queue of transactions. This is needed so that in the case
  of a block chain repair, transactions can be readded to the queue.
*/
blockchain::blockchain(synchronized_queue<transaction*>* q) {
	block* b = new block;
	b->block_number = 1;
	b->finhash = new char[15];
	std::string s = std::string("STARTINGBLOCK");
	s.copy(b->finhash, s.length(), 0);
	chain_length = 1;
	blocks_.push_front(b);
	q_ptr_ = q;
}

/* 
  Iterate over the transactions in the block and make sure none were already in the blockchain.

  \param b, the block those transaction are being verified. 
  \return boolean, indicating whether the block's transactions are all new.
*/
bool blockchain::verify_transactions(block* b) {
	// Iterate over all transactions in the block.
	for (int i = 0; i < NUM_TRANSACTIONS_PER_BLOCK; ++i) {
		// If you ever find one, return false.
		if (voted.find(b->transaction_array[i]->sender_public_key) 
			                                               != voted.end()) {
			return false;
		}
	}
	return true;
}

/* 
	Unimplemented.
*/
bool blockchain::verify_transactions() {
	return true;
}

/*
  Return a pointer to a block based on its block number
  \param n, the block number
  \return block*, the block matching that number
*/
block* blockchain::get_block(int n) {
	for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
		if ((*it)->block_number == n) {
			return *it;
		}
	}
	return NULL;
}

/*
  Return a pointer to a block based on its hash
  Assumptions: A block can be found by just comparing the hashes for equality.
  This assumptions that two distinct blocks do not share a hash. Based on  research about
  blockchain and its encryption algorithms, this is a reasonable assumption. 

  \param n, the hash
  \return block*, the block matching that hash
*/
block* blockchain::get_block(char* hash) {
	for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
		if (strcmp((*it)->finhash, hash) == 0) {
			return *it;
		}
	}
	return NULL;
}

/*
  Searches for block b in the chain, and returns the iterator to it if so.
  Assumptions: A block can be found by just comparing the hashes for equality.
  This assumptions that two distinct blocks do not share a hash. Based on  research about
  blockchain and its encryption algorithms, this is a reasonable assumption. 

  \param b, the block being searched.
  \return BlockList::iterator to the block. Will return BlockList::end() if not found
*/
BlockList::iterator blockchain::check_if_block_in_chain(block* b) {
	// Iterate over the blocks, and check if any have a matching final hash.
	for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
		if (strcmp((*it)->finhash, b->finhash) == 0) {
			return it;
		}
	}
	return blocks_.end();
}

/*
	Makes a request to all peers to looking 
*/
block* get_parent_block_from_neighbor(block* b, Client* client) {
	block* block = client->getBlock(b->block_number - 1);
	if (!block) {
		// TODO change this to just return null;
		assert(false);
	}
	return block;
}

/*
 Remove all of the transactions from block b from the set of votes. 

 /param b, the block whose transactions are being removed.
 */
void blockchain::remove_transactions_from_set(block* b) {
	for (int i = 0; i < NUM_TRANSACTIONS_PER_BLOCK; ++i) {
		voted.erase(b->transaction_array[i]->sender_public_key);
	}
	return;
}

/*
 Add all of the transactions from block b to the set of votes. 

 /param b, the block whose transactions are being added.
 */
void blockchain::add_transactions_to_set(block* b) {
	for (int i = 0; i < NUM_TRANSACTIONS_PER_BLOCK; ++i) {
		voted.insert(b->transaction_array[i]->sender_public_key);
	}
	return;
}

/*
 Add all of the transactions from block b from the queue of transactinos to process. 

 /param b, the block whose transactions are being added.
 */
void blockchain::add_transactions_to_queue(block* b) {
	for (int i = 0; i < b->max_ind; ++i) {
		q_ptr_->push(b->transaction_array[i]);
	}
}

/*
 Given a new block b, repairs the blockchain to match the history of b. 

 \param b, the block that is being added, and whose history determines the repair
 \return void
 */
void blockchain::repair_blockchain(block* b, Client* client) {
	// This list will contain the blocks that need to be added to blockchain.
	std::list<block*> blocks_to_add;
	//std::list<block*> blocks_removed;

	// Get all of the blocks that need to be added to chain
	block* current_block = b;
	BlockList::iterator it;

	while(true) {
		it = check_if_block_in_chain(current_block);
		if (it != blocks_.end())
			break;
		blocks_to_add.push_back(current_block);
		block* current_block = get_parent_block_from_neighbor(current_block, client);
	}

	// Remove all of the necessary blocks from current chain.
	// Remove their transactions from the set, and readd them
	// to the transactions queue.
	while(blocks_.begin() != it) {
		block* first_block = blocks_.front();
		remove_transactions_from_set(first_block);
		add_transactions_to_queue(first_block);
		blocks_.pop_front();
	}

	// Add the transactions for the new blocks to the set.
	for (const auto& block: blocks_to_add) {
		add_transactions_to_set(block);
	}

	// Add the actual blocks to the chain.
	// This will add the blocks_to_add to the front of blocks_.
	blocks_.splice(blocks_.begin(), blocks_to_add);

	return;

}

/* 
  Checks whether the incoming block is valid, and returns the appopriate validity code.
  This will indicate whether the block is OK to be added to the chain, has invalid transactions,
  or if its previous block doesn't match the head of the current one, meaning a blockchain
  repair may be needed.

  \param b, the block being considered
  \return block_validity_code enum element indicating the result.
*/
block_validity_code blockchain::check_block_validity(block* b) {
  // Make sure none of the votes had previously been made.

  // Make sure the new block has the correct prev block.
  block* head_block = get_head_block();
  if (b->prev_block_SHA1 != head_block->merkle_root)
    return PREV_BLOCK_NONMATCH;

  bool transaction_verification = verify_transactions(b);
  if (!transaction_verification) 
    return TRANSACTION_INVALID;
  
  // If everything checks out, return 1.
  return OK;
}

/* 
  Add a block to the head of a blockchain.
  Assumptions: This function assumes that the incoming block is OK to add to the head. It does not
  run any additional checks.

  \param b, the block being added
  \return boolean, indicating success/failure
*/
bool blockchain::add_block(block* b) {
  // TODO: Is this check still needed?
  // Call the appropriate code to make sure all of the transactions are valid.
  int block_validity_result = check_block_validity(b);
  if (block_validity_result == 1) {
    // Add block to the blockchain
    chain_length++;
    blocks_.push_front(b);
    add_transactions_to_set(b);
    return true;
  }
  return false;
}

/*
  Get's the block currently at the head of the blockchain.

  \return block*, a pointer to hhe head block
*/
block* blockchain::get_head_block(){
	// Get the head block. Don't remove it from the chain.
	if (chain_length == 0)
		return NULL;
	return blocks_.front();
}



/////////////////////////////////////////////
///// Synchronized Queue Implementation /////
/////////////////////////////////////////////

 // Initialize the queue. In particular, this should initialize the locking code.
template <class T>
void synchronized_queue<T>::init() {
	pthread_mutex_init(&lock_, NULL);
	pthread_cond_init(&cv_, NULL);
}


/* 
  Push an element of type T onto the thread-safe queue. 

  \param t: an element of type T to be pushed on the queue.
*/
template <class T>
void synchronized_queue<T>::push(T t) {
	pthread_mutex_lock(&lock_);
	queue_.push(t);
	pthread_cond_signal(&cv_);
	pthread_mutex_unlock(&lock_);
}

/* 
  Pop from the front of the queue. Block if queue is empty until an item comes in.

  \return item of type T from front of queue. 
*/
template <class T>
T synchronized_queue<T>::pop() {
	T ret;
	pthread_mutex_lock(&lock_);
	while (queue_.size() == 0) {
		pthread_cond_wait(&cv_, &lock_);
	}
	ret = queue_.back();
	queue_.pop();
	pthread_mutex_unlock(&lock_);
	return ret;
}

/* 
  Pop from the front of the queue. Return NULL if empty instead of blocking.

  \return item of type T from front of queue. 
*/
template <class T>
T synchronized_queue<T>::pop_nonblocking() {
	T ret;
	pthread_mutex_lock(&lock_);
	if (queue_.size() == 0) {
		pthread_mutex_unlock(&lock_);
		return NULL;
	}
	ret = queue_.back();
	queue_.pop();
	pthread_mutex_unlock(&lock_);
	return ret;
}

/* 
  Indicate whether queue is empty.

  \return boolean indicating whether queue is empty. 
*/
template <class T>
bool synchronized_queue<T>::empty() {
	return queue_.size() == 0;
}

template class synchronized_queue<transaction*>;
template class synchronized_queue<block*>;
template class synchronized_queue<std::string*>;
