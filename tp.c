#define _POSIX_C_SOURCE 200112L
#include "socket.h"
#include "encryptor.h"
#include "client.h"
#include "server.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR 1


int main(int argc, char *argv[]){
   if (argc < 3){
    printf("Parametros incorrectos. \n");
    return ERROR;
  }
  int is_client = strcmp(argv[1],"client"); 
  if (is_client == 0){
    if (argc < 5){
      printf("Parametros incorrectos. \n");
      return ERROR;
    }
    return client(argv[2],atoi(argv[3]),argv[4],argv[5]);
  }
  return server(atoi(argv[2]),argv[3]);
}





