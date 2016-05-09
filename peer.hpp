#ifndef __PEER_H__
#define __PEER_H__
#include <openssl/sha.h>
#include "processor.hpp"
#include "communication.hpp"
#include "rsa.hpp"
#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>
#include <openssl/rsa.h>
#include <chrono>
#include <thread>

unsigned char* SHA1(const unsigned char* s, size_t size, unsigned char* md);

/* 
	This struct defines the arguments that the communcations thread
	will receieve from the master thread
*/

struct comm_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
	synchronized_queue<std::string*>* peerq;
	blockchain* bc;
	Client* client;
};

/* 
	This struct defines the arguments that processor thread will receive
	from the master thread
*/
struct processing_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
	synchronized_queue<std::string*>* peerq;
	blockchain* bc;
	Client* client;
	std::string* own_address;
	std::string* first_peer;
};

#endif
