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

