#include <openssl/pem.h>

RSA* createRSA(unsigned char* key, int is_public_key);

RSA* createRSAWithFilename(char * filename,int is_public_key);

int public_encrypt(unsigned char * data, int data_len,  char * key_file, unsigned char *encrypted);
int private_decrypt(unsigned char * enc_data, int data_len,  char * key_file, unsigned char *decrypted);
int private_encrypt(unsigned char * data, int data_len,  char * key_file, unsigned char *encrypted);
int public_decrypt(unsigned char * enc_data, int data_len,  char * key_file, unsigned char *decrypted);