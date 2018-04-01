#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <string.h>

typedef struct {
  int socket;
  const char * host_name;
  unsigned short port;
  struct addrinfo * addrinfo;
  bool is_connected;
} socket_t;


int socket_create(socket_t *self, const char * host_name, unsigned short port);
int socket_destroy(socket_t *self);
int socket_connect(socket_t *self);
int socket_send_message(socket_t *self, const char* buffer, size_t length);
int socket_recive_message(socket_t *self, char* buffer, size_t length);
int socket_create(socket_t *self, const char * host_name, unsigned short port);
int socket_bind_and_listen(socket_t *self);
int socket_accept(socket_t *self, socket_t* accepted_socket);

#endif
 
