#define _POSIX_C_SOURCE 200112L
#include "socket.h"
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

#define MAX_PORT_LEN 6


int socket_creation(socket_t *self);
int addrinfo(socket_t *self);
int get_digits(int num);

//_____________________________________________________________________ privates

int get_digits(int num){
    int digits = 0;
    while ( num > 0 ) {
        num /= 10;
        digits++;
    }
    return digits;
}

int addrinfo(socket_t *self){
    int digitos = get_digits(self->port);
    char str_port[MAX_PORT_LEN];
    snprintf(str_port,digitos +1 , "%d", self->port);
  

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo)); 
    hints.ai_family = AF_INET;       //IPv4 
    hints.ai_socktype = SOCK_STREAM; //TCP 
    hints.ai_flags = 0; 

    struct addrinfo *addrinfoNode;
    

    int addrinfo_returnvalue = 
            getaddrinfo(self->host_name, str_port, &hints, &addrinfoNode);

    if (addrinfo_returnvalue != SUCCESS){
      printf("Error in getaddrinfo: %s\n", gai_strerror(addrinfo_returnvalue));
      return ERROR;
    }
    self->addrinfo = addrinfoNode;
    return SUCCESS;
}

int socket_creation(socket_t *self){
  int socket_num = 0;
  while (self->addrinfo != NULL) {
    socket_num = //
      socket(self->addrinfo->ai_family, 
          self->addrinfo->ai_socktype, self->addrinfo->ai_protocol);
    if (socket_num != -1) {
      self-> socket = socket_num;
      return SUCCESS;
    }
    self->addrinfo = self->addrinfo->ai_next;
  }
  return ERROR;
}



//_________________________________________________________________


int socket_create(socket_t *self, const char * host_name, unsigned short port){
    self->host_name = host_name;
    self->port = port;
    self->is_connected = false;

    if (addrinfo(self) != SUCCESS || socket_creation(self) != SUCCESS) { 
      return ERROR;
    }

    int val = 1;
    if (setsockopt(self->socket, SOL_SOCKET, SO_REUSEADDR, &val, //
      sizeof(val)) == -1) {
        printf("Error in reuse socket: %s\n", strerror(errno));
        close(self->socket);
        return ERROR;
    }
    return SUCCESS;
  }


int socket_accept(socket_t *self, socket_t* accepted_socket){
  int socket_accepted = accept(self->socket, NULL, NULL);
  if (socket_accepted == -1){
    return ERROR;
  }
  accepted_socket-> socket = socket_accepted;
  return SUCCESS;
}

int socket_destroy(socket_t *self){
  freeaddrinfo(self->addrinfo);
  shutdown(self->socket, SHUT_RDWR);

  close(self->socket);
  return SUCCESS;
}

int socket_recive_message(socket_t *self, char* buffer, size_t size){
    int total_received = 0;
    int bytes_recived = 0;

    while (total_received < size) {
        bytes_recived = 
            recv(self->socket, &buffer[total_received], 1, MSG_NOSIGNAL);

        if (bytes_recived < 0) { 
          printf("Error recibing info\n");
          return -1;
        } else if (bytes_recived == 0) { 
          return total_received;
        } else {
          total_received += bytes_recived;
        }
    }
    return total_received;
}

int socket_send_message(socket_t *self, const char* buffer, size_t size){
  int total_sent = 0;
  int bytes_sent = 0;
  bool valid_socket = true;

  while (total_sent < size && valid_socket) {
    bytes_sent = //
        send(self->socket, &buffer[total_sent], size-total_sent, MSG_NOSIGNAL);
    if (bytes_sent == 0) {
      valid_socket = false;
    } else if (bytes_sent < 0) {
      valid_socket = false;
    } else {
      total_sent += bytes_sent;
    }
  }

  if (valid_socket) {
    return total_sent;
  } else {
    return -1;
  }
}

int socket_connect(socket_t *self){
  int create_returnvalue = 0;
  while (self->addrinfo != NULL && self->is_connected == false) {
    if (create_returnvalue == 1) {
      return ERROR;
    }
    int connection_returnvalue = //
    connect(self->socket, self->addrinfo->ai_addr, self->addrinfo->ai_addrlen);
    self->is_connected = (connection_returnvalue != -1);
    if (self->is_connected){
      break;
    }
    printf("Error in socket connection: %s\n", strerror(errno));
    close(self->socket);
    self->addrinfo = self->addrinfo->ai_next;
    create_returnvalue = socket_creation(self);
  }
  if (self->is_connected == false) {
    return ERROR; 
  }
  return SUCCESS;
}


int socket_bind_and_listen(socket_t *self){
    int bindReturnValue = //
      bind(self->socket, self->addrinfo->ai_addr, self->addrinfo->ai_addrlen);
    if (bindReturnValue == -1) {
        printf("%i\n",self-> port);
        printf("Error in bind: %s\n", strerror(errno));
        return ERROR;
    }
    int listenReturnValue = listen(self->socket, 20);
    if (listenReturnValue == -1) {
        printf("Error in listen: %s\n", strerror(errno));
        return ERROR;
    }
    self-> is_connected = true;
    return SUCCESS;
}
