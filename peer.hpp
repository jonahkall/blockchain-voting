#ifndef __PEER_H__
#define __PEER_H__
#include <openssl/sha.h>

unsigned char* SHA1(const unsigned char* s, size_t size, unsigned char* md);

#endif