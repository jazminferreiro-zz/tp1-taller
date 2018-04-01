#define _POSIX_C_SOURCE 200112L
#include "client.h"
#include "socket.h"
#include "encryptor.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#define CLIENT_CHUNK_LEN 64
#define CHUNKS 1

#define SUCCESS 0
#define ERROR 1



  
int client(const char * ip, const int port, 
  const char *key, const char * file_name){
  socket_t * skt = (socket_t *)malloc(sizeof(socket_t));
  if (socket_create(skt, ip, port) != SUCCESS){
    printf("Error in socket creation\n");
    return ERROR;
  }
  
  if (socket_connect(skt) != SUCCESS || skt->socket < 1){
    socket_destroy(skt);
    free(skt);
    return ERROR;
  }

  encryptor_t * client_encryptor = encryptor_create(key);

  FILE * file = fopen(file_name, "rb");
  if (file == NULL){
    printf("Error opening file %s\n", file_name);
    socket_destroy(skt);
    free(skt);
    return ERROR; // error
  }
  fseek(file,0,SEEK_END);
  int file_len = ftell(file);
  
  client_send_file_to_server(skt, client_encryptor, file, file_len);

  encryptor_destroy(client_encryptor);
    
  fclose(file);
 
  socket_destroy(skt);
  free(skt);
  return SUCCESS;
}

int client_send_file_to_server(socket_t * skt, 
          encryptor_t * client_encryptor,FILE * file, int file_len){
  int bytes_sent = 0;
  int total_sent = 0;
  int chunks_read = 0;
  char request[CLIENT_CHUNK_LEN+1];

  while ( total_sent < file_len && bytes_sent >= 0 ){
    memset(request, 0, CLIENT_CHUNK_LEN+1);

    fseek(file, total_sent, SEEK_SET);
    chunks_read = fread(request, CLIENT_CHUNK_LEN, CHUNKS, file);
    if (chunks_read != CHUNKS && feof(file) == 0){
      continue;
    }
    
    int request_len = CLIENT_CHUNK_LEN;
    if (file_len - total_sent < CLIENT_CHUNK_LEN){
      request_len = file_len - total_sent;
    }

    encryptor_encrypt(client_encryptor,request, request_len);
    bytes_sent = socket_send_message(skt, request, request_len);
    total_sent += bytes_sent;
  }
  return SUCCESS;
}
