#include "peer.hpp"
#include "processor.hpp"
#include "communication.hpp"
//#include "sha1.h"
#include <openssl/sha.h>

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

	while(true) {
		// Call pop_nonblocking. If not null, add the block to the blockchain, clear progress
		// on new block, add everything from the block to the set
		block* b = ptap->bq->pop_nonblocking();
		if (b) {
			// TODO: clear progress somehow

			switch(bc.check_block_validity(b)) {
				case OK:
					//bc.add();
					break;
				case PREV_BLOCK_NONMATCH:
					break;
				case TRANSACTION_INVALID:
					break;
			}

			//if ()
		}
		// Get a transaction from the transaction queue.

			// Check if already in the block chain (via the unordered set). If so, throw it out.

		// add to set of things we are trying to turn into a block

		// try to create a block x times.


	}


	return NULL;
}

int main () {

	// char buffer[20];
 //  string s = string("Some-Sample-Input");
 //  s.copy(buffer, 10, 0);

	//char *m = sha1(buffer);

	//printf("The SHA1 Value is: %s\n", m);

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