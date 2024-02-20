#ifndef __SOCK_HELPER_H__
#define __SOCK_HELPER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define LISTENQ 8

/**
 * Establece una conexión (devolviendo el descriptor de archivo del socket correspondiente),
 * con un servidor que se encuentra escuchando en la dirección 'hostname:port'
 */
int open_clientfd(char *hostname, char *port);

/**
 * Configura un servidor para que escuche por todas las interfaces disponibles en el
 * puerto indicado.
 */
int open_listenfd(char *port);

#endif