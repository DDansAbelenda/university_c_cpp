CC := gcc
LIBRIO := -L./librio/ -lrio

.PHONY: dynamic

libs:
	cd librio && $(MAKE) all

threads: threadpool.o

sockets: sock_helper.o

dynamic:
	echo "What about this?"
	cd dynamic && $(MAKE) all

server: libs threads sockets dynamic
	$(CC) -o rpc_server rpc_sock_server.c threadpool.o sock_helper.o ${LIBRIO} -ldl -pthread

client: libs sockets
	$(CC) -o rpc_client rpc_sock_client.c sock_helper.o ${LIBRIO}

all: client server

clean:
	cd librio && $(MAKE) clean
	cd dynamic && $(MAKE) clean
	rm -rf *.o *.a rpc_server rpc_client
