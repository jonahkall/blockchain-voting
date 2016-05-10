# OneVote - Jonah, Matt, Ankit, Willy

## Installation
First, clone this repository.

### Dependencies
- grpc C++ bindings
- protobuf
- openssl (w/ C headers)
- C++11
- g++ compiler

## Usage

### Compilation
To compile the miner, type `make miner`.
To compile the voting booth, type `make voting-booth`
To compile both, just type `make` or `make all`
### Running

- To run the miner: `./runpeer2 <YOUR_IP_ADDRESS>:<YOUR_PORT> 54.164.190.217:50051`. Here, the first argument should contain your ip address and port. The second argument is the ip address and port of one peer.
- To run the voting booth, `./voting-booth booth 52.87.164.121:50051 <YOUR_PUBLIC_KEY_FILE> <VOTE>`. Here, the first argument should be "booth", the second should be the ip address of one peer, the third is the filename of the voters public key, and vote is the vote cast.

### Files

- Makefile: This is used for compilation.
- server.cpp and server.hpp: These files allow peers to receive transmitted transactions and blocks as well as other requests from the P2P network.
- client.cpp and client.hpp: These files allow peers to send transactions and blocks as well as other requests to the P2P network.
- encode_helpers.cpp and encode_helpers.hpp: These files contain helper functions which help package blocks and transactions into messages which can be sent over the wire.
- processor.cpp and processor.hpp: This code is the implementation of all the backend processing code, including the code for blocks, transactions, and blockchains, farming out requests for information to the communication code.
- peer.cpp and peer.hpp:  This code is what would be run by a miner peer in our network, launching communication and mining threads to attempt to discover blocks and communicate with other peers.
- rsa.cpp and rsa.hpp: This code implements the RSA cryptographic protocol, which we use as the PKI allowing users to verify validity of signed transactions (votes).
- voting-booth.cpp and voting-booth.hpp: These files implement the voting booth service, which allows voters to broadcast their public key and their vote to the network.
- keys/*.pem: These files are the public keys for all voters in our system (artificially generated, in real life these would be input by the voters at the booth).
- protos/node.proto: These are the messages for all of the serializable data structures we want to send over the wire, including blocks, transactions, their associated request types, and messages for finding and communicating with peers.

