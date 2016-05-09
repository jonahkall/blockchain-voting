#include "rsa.hpp"
#include <stdio.h>

/* 
createRSA

Creates an RSA object from a public key. Use this if the key is loaded into a variable.

\param key: This the public or private key that is being used to make the RSA object
\param is_public_key: This should be 1 if this is a public key, or 0 otherwise.

\return RSA*: A pointer to an RSA object. 
*/
RSA* createRSA(unsigned char* key, int is_public_key) {
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(is_public_key)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
 
    return rsa;
};

/**
createRSAWithFilename

Creates an RSA object from a public key in a file. Use this if the key is in a file.

\param filename: This filename of the file containing the public or private key that is being used to make the RSA object
\param is_public_key: This should be 1 if this is a public key, or 0 otherwise.

\return RSA*: A pointer to an RSA object. 
*/
RSA * createRSAWithFilename(char * filename,int is_public_key)
{
    FILE * fp = fopen(filename,"rb");
 
    if(fp == NULL)
    {
        printf("Unable to open file %s \n",filename);
        return NULL;    
    }
    RSA *rsa= RSA_new() ;
 
    if(is_public_key)
    {
        rsa = PEM_read_RSA_PUBKEY(fp, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_RSAPrivateKey(fp, &rsa,NULL, NULL);
    }

    fclose(fp);
 
    return rsa;
}

int padding = RSA_PKCS1_PADDING;

/**
public_encrypt

This function encrypts data using a a public key. 

\param data, the data to be encrypted.
\param data_len, the length of the data.
\key_file, the filename of the file with the key.
\encrypted, a pointer to the location where the encrypted string will go.

\returns int, indicating success or failure.

*/
int public_encrypt(unsigned char * data,int data_len,char * key_file, unsigned char *encrypted) {
    RSA * rsa = createRSAWithFilename(key_file,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}

/**
private_decrypt

This function decrypts data using a private key, which was encrypted with the corresponding public key.

\param enc_data, the data that was encrypted.
\param data_len, the length of the data.
\key_file, the filename of the file with the key.
\decrypted, a pointer to the location where the decrypted data will go.

\returns int, indicating success or failure.

*/
int private_decrypt(unsigned char * enc_data,int data_len,char * key_file, unsigned char *decrypted) {
    RSA * rsa = createRSAWithFilename(key_file,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

/**
private_encrypt

This function encrypts data using a private key. This is used to create a digital signature.

\param data, the data to be encrypted.
\param data_len, the length of the data.
\key_file, the filename of the file with the key.
\encrypted, a pointer to the location where the encrypted string will go.

\returns int, indicating success or failure.

*/
int private_encrypt(unsigned char * data,int data_len,char * key_file, unsigned char *encrypted) {
    RSA* rsa = createRSAWithFilename(key_file,0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

/**
public_decrypt

This function decrypts a digital signature using a public key, which was encrypted with the corresponding private key.

\param enc_data, the data that was encrypted.
\param data_len, the length of the data.
\key_file, the filename of the file with the key.
\decrypted, a pointer to the location where the decrypted data will go.

\returns int, indicating success or failure.

*/
int public_decrypt(unsigned char * enc_data,int data_len,char * key_file, unsigned char *decrypted) {
    RSA* rsa = createRSAWithFilename(key_file,1);
    int result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

