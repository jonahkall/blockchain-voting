#include "peer.hpp"
#include "processor.hpp"
#include "communication.hpp"
#include "rsa.hpp"
//#include "sha1.h"
#include <openssl/sha.h>
#include <openssl/rsa.h>



using namespace std;

struct comm_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
};

struct processing_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
};


void* comm_thread (void* arg) {
	comm_thread_args* ctap = (comm_thread_args *) arg;
	cout << "Hello from comm thread\n";
	return NULL;
}

void* processing_thread(void* arg) {
	processing_thread_args* ptap = (processing_thread_args *) arg;
	cout << "Hello from processing thread\n";

	blockchain bc;
	bc.chain_length = 0;

	// When this variable is true, we have a full set of
	// transactions to try to make a block with, otherwise we do not, so we are waiting
	// on that
	bool quotafull = false;
	block* new_block = new block;
	int txns_in_current_block;

	// This is an example of how code will be encrypted and decrypted. 

	// Create a message to encrypt
	char data[2048/8] = "This is the message to encrypt/decrypt. If you see this, it works!"; 
	// Create buffers that will store the encrypted and decrypted results.
	unsigned char encrypted[4098];
	unsigned char decrypted[4098];

	// Encrypt and store in "encrypted". Get the encrypted_length
	int encrypted_length = private_encrypt((unsigned char *) data, strlen(data), "private.pem", encrypted);
	// Decrypt and store in "decrypted"
	public_decrypt(encrypted, encrypted_length, "public.pem", decrypted);
	// print message to stdout. It should match what is in data above.
	cout << decrypted << endl;

	while(true) {
		// Call pop_nonblocking. If not null, add the block to the blockchain, clear progress
		// on new block, add everything from the block to the set
		block* b = ptap->bq->pop_nonblocking();
		if (b) {
			// TODO: clear progress somehow

			switch(bc.check_block_validity(b)) {
				case OK:
					bc.add_block(b);
					break;
				case PREV_BLOCK_NONMATCH:
					// Check if this block number is higher than ours, in which
					// case we need to accept that chain instead of ours
					if (b->block_number > bc.get_head_block()->block_number) {
						// Do important shit here
						// send requests for missing blocks, and align them to current
						// blockchain
						// O(n^2) algorithm for alignment
						bc.repair_blockchain(b);
					}
					else {
						continue;
					}
					break;
				case TRANSACTION_INVALID:
					continue;
					break;
			}

			//if ()
		}
		// Get a transaction from the transaction queue.
		if (ptap->tq->empty() && txns_in_current_block < NUM_TRANSACTIONS_PER_BLOCK)
			transaction* new_trans = ptap->tq->pop();
		
		// Check if already in the block chain (via the unordered set). If so, throw it out.
		// if (bc.voted.find() != bc.voted().end()) {
		// 	continue;
		// }
		// add to set of things we are trying to turn into a block

		// try to create a block x times.


	}


	return NULL;
}

int main () {

	const char str[] = "Original String";
  unsigned char hash[SHA_DIGEST_LENGTH]; // == 20

  SHA1((const unsigned char*)str, sizeof(str) - 1, hash);

  for (int i = 0; i < 20; ++i)
		printf("%02X", hash[i]);
	printf("\n");

	pthread_t comm_t;
	pthread_t processing_t;

	synchronized_queue<transaction*> tq = synchronized_queue<transaction*>();
	tq.init();

	synchronized_queue<block*> bq = synchronized_queue<block*>();
	bq.init();

	comm_thread_args cta;
	cta.tq = &tq;
	cta.bq = &bq;

	processing_thread_args pta;
	pta.tq = &tq;
	pta.bq = &bq;

	pthread_create(&comm_t, NULL, comm_thread, &cta);
	pthread_create(&processing_t, NULL, processing_thread, &pta);

	pthread_join(comm_t, NULL);
	pthread_join(processing_t, NULL);

	return 0;
}




