#ifndef __CLIENT_H__
#define __CLIENT_H__

#define NOT_AN_IP_TOKEN "dummy"

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>
#include <regex>

#include <grpc++/grpc++.h>

#include "node.grpc.pb.h"
#include "encoding_helpers.hpp"
#include "processor.hpp"
class block;
struct transaction;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using onevote::Empty;
using onevote::BlockMsg;
using onevote::TransactionMsg;
using onevote::AddrRequest;
using onevote::AddrResponse;
using onevote::TransactionRequest;
using onevote::BlockRequest;
using onevote::Miner;

/*
    Implements a specific gRPC client that connects with Servers running
    the Miner::Service
*/
class SinglePeerClient {
  public:
    // Initializes the SinglePeerClient which requires a gRPC channel an address to connect to
    // and the peer's own client.
    SinglePeerClient(std::shared_ptr<Channel> channel, std::string my_addr, std::string addr);

    // Broadcasts a block to the peer
    Status BroadcastBlock(block* block);

    // Broadcasts a transaction to the peer
    Status BroadcastTransaction(transaction* transaction);

    // Asks for a block from the peer, returns NULL if they don't return
    // or if they don't have the plock
    block* GetBlock(char* block_hash);

    // Returns the address of the peer
    std::string* peerAddr();

    // Gets all of the peers that the peer has
    AddrResponse GetAddr();

    // Sends a heartbeat to the peer, returns true if its
    // alive
    bool GetHeartbeat();

  private:
    // The client's IP_ADDRESS:PORT
    std::string my_addr_;

    // The peer's IP_ADDRESS:PORT
    std::string addr_;

    // Used for gRPC
    std::unique_ptr<Miner::Stub> stub_;
};

/*
    This class defined a communications Client API that is consumed by a processor.
    It implements a wrapper around a list of SinglePeerClients.
*/
class Client {
  public:
    // Initializes the Client, it must know its own address and an initial
    // peer to make a connection to. It will then bootstrap from that peer.
    Client(std::string own_address, std::string first_peer);

    // Sends a block to all the peers in its network
    void BroadcastBlock(block* block);

    // Sends a transaction to all the peers in its network
    void BroadcastTransaction(transaction* transaction);

    // Sends a heartbeat to all peers in its network and returns
    // the number of peers that are still alive.
    int checkHeartbeats();

    // Asks peers in its network for their peers and then 
    // adds them to client's peer network. Returns the number
    // of peers actve in its network.
    int bootstrapPeers();

    // Returns a peers list to be consumed by server
    std::list<std::string*>* getPeersList();

    // Adds a new peer to client's neetwork and ensures
    // invariants.
    void addNewPeer(std::string addr);

    // Asks each of its peers in the network for
    // a specific block until it finds it. Returns NULL
    // otherwise
    block* getBlock(char* block_hash);

  private:
    // The first peer that the client connects to
    std::string first_peer_;

    // The client's own address, needed when sending
    // GetAddr requests
    std::string my_address_;

    // List of peers that this is connected ot
    // AddNewPeer will ensure that there are no duplicates
    // and that each peer has a valid IP.
    std::list<SinglePeerClient*> peer_clients_;

    // Callback to be used for checkHeartbeats
    bool successHearbeat(const SinglePeerClient*& peer_client);

    // Used to make logs
    void clientLog(std::string message);
};

#endif
