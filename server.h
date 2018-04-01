#ifndef __SERVER_H__
#define __SERVER_H__

#define _POSIX_C_SOURCE 200112L

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

int server(const int port, const char * key);
int server_accept_client(
	socket_t * skt, encryptor_t * server_decryptor, FILE * file);

#endif
