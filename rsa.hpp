#include <openssl/pem.h>


/* 
 createRSA

 Creates an RSA object from a public key. Use this if the key is loaded into a variable.

 \param key: This the public or private key that is being used to make the RSA object
 \param is_public_key: This should be 1 if this is a public key, or 0 otherwise.

 \return RSA*: A pointer to an RSA object. 
 */
RSA* createRSA(unsigned char* key, int is_public_key);

/* 
 createRSAWithFilename

 Creates an RSA object from a public key in a file. Use this if the key is in a file.

 \param filename: This filename of the file containing the public or private key that is being used to make the RSA object
 \param is_public_key: This should be 1 if this is a public key, or 0 otherwise.

 \return RSA*: A pointer to an RSA object. 
 */RSA* createRSAWithFilename(char * filename,int is_public_key);

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