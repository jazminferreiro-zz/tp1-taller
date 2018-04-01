#ifndef __CLIENT_H__
#define __CLIENT_H__

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

int client(const char * ip, const int port, const char * key, //
	const char * file_name);
int client_send_file_to_server(socket_t * skt,encryptor_t * client_encryptor,//
	FILE * file, int file_len);

#endif
