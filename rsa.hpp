#include <openssl/pem.h>


// Use this if the keys are loaded.
RSA* createRSA(unsigned char* key, int is_public_key);

// Use this if the keys are stored in a file.
RSA* createRSAWithFilename(char * filename,int is_public_key);

/////////////////////////////////////////
// Used to send an encrypted message to the person who's public key you use.
/////////////////////////////////////////
int public_encrypt(unsigned char * data, int data_len,  char * key_file, unsigned char *encrypted);
int private_decrypt(unsigned char * enc_data, int data_len,  char * key_file, unsigned char *decrypted);


/////////////////////////////////////////
// Used to create a digital signature
/////////////////////////////////////////
int private_encrypt(unsigned char * data, int data_len,  char * key_file, unsigned char *encrypted);
int public_decrypt(unsigned char * enc_data, int data_len,  char * key_file, unsigned char *decrypted);