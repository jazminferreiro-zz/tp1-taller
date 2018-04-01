#define _POSIX_C_SOURCE 200112L
#include "server.h"
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


#define SERVER_CHUNK_LEN 50


#define SUCCESS 0
#define ERROR 1

#define OUTPUT_FILE "output.txt"


int server(const int port, const char * key) {
  socket_t * skt = (socket_t *)malloc(sizeof(socket_t)); 
  if (socket_create(skt, NULL, port) != SUCCESS){
    return ERROR;
  }

  if (socket_bind_and_listen(skt) != SUCCESS){
    socket_destroy(skt);
    return ERROR;
  }

  encryptor_t * server_decryptor = encryptor_create(key);

  FILE * file = fopen(OUTPUT_FILE, "wb");
  if (file == NULL){
    socket_destroy(skt);
    free(skt);
    return ERROR;
  }

  int client_listening =  server_accept_client(skt, server_decryptor, file);

  fputc('\n',file);
  fclose(file);
  
  encryptor_destroy(server_decryptor);

  socket_destroy(skt);
  free(skt);

  return client_listening;
}

int server_accept_client(socket_t * skt, 
  encryptor_t * server_decryptor, FILE * file){
  bool continue_running = true;
  bool is_socket_accepted_valid = true;
  char chunk[SERVER_CHUNK_LEN+1];
  socket_t * peer_socket = (socket_t *)malloc(sizeof(socket_t));
    
  if (socket_accept(skt, peer_socket) != SUCCESS) {
    printf("Error in accept client: %s\n", strerror(errno));
    continue_running = false;
    is_socket_accepted_valid = false;
  } else {
    while (continue_running){
      memset(chunk, 0, SERVER_CHUNK_LEN+1);
      int bytes_received = 
        socket_recive_message(peer_socket, chunk, SERVER_CHUNK_LEN); 
      if (bytes_received <= 0){
        break;
      }
      encryptor_encrypt(server_decryptor,chunk, bytes_received);
      fwrite(chunk, bytes_received, 1, file);
    }

    socket_destroy(peer_socket);
    free(peer_socket);
  }

  if (is_socket_accepted_valid) {
    return ERROR;
  } else { 
    return SUCCESS;
  }
}
