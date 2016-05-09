#include "voting-booth.hpp"

int main (int argc, char** argv) {
	if (argc != 5) {
		std::cout << "Usage: ./voting-booth your-ip-address:port first-peer-address:port publickeyfile vote";
		return 1;
	}

	std::string own_address(argv[1]);
	std::string first_peer(argv[2]);

	std::ifstream filestream(argv[3]);
	std::string publickey((std::istreambuf_iterator<char>(filestream)),
                 std::istreambuf_iterator<char>());
	std::string vote(argv[4]);

	std::cout << own_address << std::endl;
	std::cout << first_peer << std::endl;
	std::cout << publickey << std::endl;
	std::cout << vote << std::endl;

	Client* client = new Client("VotingBoothAddress", first_peer);
	//client->bootstrapPeers();

	transaction* t = new transaction;
	t->sender_public_key = publickey;
	t->vote = vote;
	auto unix_timestamp = std::chrono::seconds(std::time(NULL));
	t->timestamp = unix_timestamp.count();
	std::cout << std::setprecision(12) << t->timestamp << std::endl;
}
