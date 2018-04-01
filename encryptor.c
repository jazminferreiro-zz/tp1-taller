#include "encryptor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void encryptor_ksa(encryptor_t * self);
void encryptor_prga(encryptor_t * self, int len);
void encryptor_swap_S(encryptor_t * self, int i, int j);
void stderr_keystream_hexa_uppercase(encryptor_t *self , int len);
void stdin_input_hexa_lowercase(char * input, int len);




//key-scheduling
void encryptor_ksa(encryptor_t * self){
  for (int i=0; i < 256; i++){
    self->S[i] = i;
  }
  int keyLen = strlen(self->key);

  int j = 0;
  for (int i=0; i < 256; i++){
    j = (j+ self->S[i] + self->key[i % keyLen]) %256;
    encryptor_swap_S(self,i,j);
  }
}

//programacion pseudo-aleatoria
void encryptor_prga(encryptor_t * self, int len){
  int  k;
  for (int index =0; index <len; index++){
    self->i = (self->i +1) % 256;
    self->j = (self->j +self->S[self->i]) % 256;
    encryptor_swap_S(self, self->i, self->j);

    k = self->S[(self->S[self->i]+self->S[self->j]) % 256];
    self->keystream[index] = k;
  }
}

void encryptor_swap_S(encryptor_t * self, int i, int j){
  int aux = self->S[i];
  self->S[i]= self->S[j];
  self->S[j] = aux; 
}



void stderr_keystream_hexa_upperrcase(encryptor_t *self , int len){
  for (int i=0; i < len; i++){
    fprintf(stderr, "%02X", self->keystream[i] & 0xff);
  }
}

void stdin_input_hexa_lowercase(char * input, int len){
  for (int i=0; i < len; i++){
    printf("%02x", input[i] & 0xff);
  }
  printf("\n");
}

void encryptor_encrypt(encryptor_t * self, char * input, int len){
  self-> keystream = (char *)malloc(sizeof(char)*len);
  encryptor_prga(self, len);
  for (int i = 0; i < len; i++){
    input[i] = input[i] ^ self->keystream[i];
  }
  stdin_input_hexa_lowercase(input, len);
  stderr_keystream_hexa_upperrcase(self,len);
  free(self->keystream);
}

encryptor_t * encryptor_create(const char * key){
  encryptor_t * self = (encryptor_t *)malloc(sizeof(encryptor_t));
  self->key = key;
  self-> i = 0;
  self-> j = 0;
  encryptor_ksa(self);
  return self;
}

void encryptor_destroy(encryptor_t * self){
  free(self);
}

