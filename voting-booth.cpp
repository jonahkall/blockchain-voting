#include "voting-booth.hpp"

/**
 	This implements the voting booth function. Essentially all this requires is creating a client connection to
 	one peer, getting peers from that peer, and then broadcasting a transaction over the network. 

 	This creates a Transaction object based on the public key file and vote specified as command line arguments.
 	Assumptions:
 		The IP address and port of one peer is known.
 		Your own public key is known.
 		You know who you are voting for.

 */
int main (int argc, char** argv) {

	// Check headers
	if (argc != 5) {
		std::cout << "Usage: ./voting-booth your-ip-address:port first-peer-address:port publickeyfile vote";
		return 1;
	}

	// Read the command line argument. 
	std::string own_address(argv[1]);
	std::string first_peer(argv[2]);
	std::ifstream filestream(argv[3]);
	std::string publickey((std::istreambuf_iterator<char>(filestream)),
                 std::istreambuf_iterator<char>());
	std::string vote(argv[4]);


	transaction* t = new transaction;
	t->sender_public_key = publickey;
	t->vote = vote;
	auto unix_timestamp = std::chrono::seconds(std::time(NULL));
	t->timestamp = unix_timestamp.count();

	// Print the args to the commannd line to check them.
	// std::cout << own_address << std::endl;
	// std::cout << first_peer << std::endl;
	// std::cout << publickey << std::endl;
	// std::cout << vote << std::endl;
	// std::cout << std::setprecision(12) << t->timestamp << std::endl;

	Client* client = new Client(NOT_AN_IP_TOKEN, first_peer);
	// Uncomment these when bootstrap peers works.
	client->bootstrapPeers();
	client->BroadcastTransaction(t);
	std::cout << "Your vote has been cast!" << std::endl;
}
