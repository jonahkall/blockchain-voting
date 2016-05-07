#include "peer.hpp"
#include "processor.hpp"
#include "communication.hpp"

struct comm_thread_args {

};

struct processing_thread_args {

};

void* comm_thread (void* arg) {
	return NULL;
}

void* processing_thread(void* arg) {
	return NULL;
}

int main () {
	pthread_t comm_t;
	pthread_t processing_t;

	transaction_queue tq = transaction_queue();
	tq.init();

	pthread_create(&comm_t, NULL, comm_thread, NULL);
	pthread_create(&processing_t, NULL, processing_thread, NULL);

	pthread_join(comm_t, NULL);
	pthread_join(processing_t, NULL);

	return 0;
}