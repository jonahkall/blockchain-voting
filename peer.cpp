#include "peer.hpp"
#include "processor.hpp"
#include "communication.hpp"

using namespace std;

struct comm_thread_args {
	synchronized_queue<transaction>* q;
};

struct processing_thread_args {
	synchronized_queue<transaction>* q;
};

void* comm_thread (void* arg) {
	comm_thread_args* ctap = (comm_thread_args *) arg;
	cout << "Hello from comm thread\n";
	return NULL;
}

void* processing_thread(void* arg) {
	processing_thread_args* ptap = (processing_thread_args *) arg;
	cout << "Hello from processing thread\n";
	//td::unordered_set voters;

	while(true) {
		// Check if the block_queue is empty. If not, add the block as necessary. 

		// Get a transaction from the transaction queue.
			// Check if already in the block chain. If so, throw it out.
		// If not, add to set of voters. 
		// 


	}


	return NULL;
}

int main () {
	pthread_t comm_t;
	pthread_t processing_t;

	synchronized_queue<transaction> tq = synchronized_queue<transaction>();
	tq.init();

	comm_thread_args cta;
	cta.q = &tq;

	processing_thread_args pta;
	pta.q = &tq;

	pthread_create(&comm_t, NULL, comm_thread, &cta);
	pthread_create(&processing_t, NULL, processing_thread, &pta);

	pthread_join(comm_t, NULL);
	pthread_join(processing_t, NULL);

	return 0;
}