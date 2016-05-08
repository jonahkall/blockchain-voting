#include "rsa.hpp"
#include <stdio.h>

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

int public_encrypt(unsigned char * data,int data_len,char * key_file, unsigned char *encrypted) {
    RSA * rsa = createRSAWithFilename(key_file,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int private_decrypt(unsigned char * enc_data,int data_len,char * key_file, unsigned char *decrypted) {
    RSA * rsa = createRSAWithFilename(key_file,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

int private_encrypt(unsigned char * data,int data_len,char * key_file, unsigned char *encrypted) {
    RSA* rsa = createRSAWithFilename(key_file,0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

int public_decrypt(unsigned char * enc_data,int data_len,char * key_file, unsigned char *decrypted) {
    RSA* rsa = createRSAWithFilename(key_file,1);
    int result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

