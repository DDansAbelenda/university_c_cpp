#ifndef __RIO_PRIV_H__
#define __RIO_PRIV_H__

#include "rio.h"

/**
 * La cabecera que se usa para declarar algunos detalles que son internos
 * a la implementación de la biblioteca.
 * 
 * Las funciones y tipos de datos que se exponen aquí no están pensadas para ser
 * consumidas por los usuarios finales, sino que son un detalle de implementación
 * y no se supone que sean visibles.
*/

ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n);

#endif