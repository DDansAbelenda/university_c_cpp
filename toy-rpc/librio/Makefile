# Una lista de los objetos de los cuales depende la biblioteca que
# vamos a construir
RIO_LIB_DEPS := \
	rio_read.o \
	rio_readinitb.o \
	rio_readlineb.o \
	rio_readn.o \
	rio_readnb.o \
	rio_writen.o

# Configuramos el compilador de C
# que se va a usar por defecto.
CC := gcc

# El objetivo definido aquí produce nuestra
# biblioteca estática.
# Al establecer la lista de objetos como dependecias
# make define reglas implícitas que permiten compilar los
# archivos fuente correspondientes en los ficheros de código
# objeto necesarios.
librio: ${RIO_LIB_DEPS}
	ar -crs librio.a ${RIO_LIB_DEPS}

# Invocar 'make clean' limpiará todos
# los resultados de nuestro trabajo para poder
# crear la biblioteca desde cero.
clean:
	rm -f *.a *.o

# No es necesario en este caso, pero esta es
# la opción que normalmente se ejecuta cuando invocamos
# 'make' sin especificar un objetivo, de lo contrario
# se ejecuta el primero que se encuentre.
all: librio
