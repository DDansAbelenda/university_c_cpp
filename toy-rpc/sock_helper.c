#include "sock_helper.h"

int open_clientfd(char *hostname, char *port)
{
    int clientfd;
    struct addrinfo hints, *listp, *p;

    // Limpia los contenidos de la estructura donde se guardan las direcciones */
    memset(&hints, 0, sizeof(struct addrinfo));

    // Lista las posibles direcciones del servidor.
    // La configuración está indicando que se quiere una variante TCP
    // (orientada a la conexión, indicado por SOCK_STREAM) y que el puerto
    // que vamos a usar está referenciado por un valor numérico.
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;

    // Busca posibles coincidencias de servidor para el valor de hostname:port,
    // respetando la configuración que establecimos en hints.
    // listp contendrá la lista de posibles resultados.
    getaddrinfo(hostname, port, &hints, &listp);

    // Iteramos sobre la lista de resultados, tratando de conectar a alguno de ellos
    for (p = listp; p; p = p->ai_next)
    {
        // Tratamos de contactar al socket correspondiente
        clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        // Si falla, probamos con el siguiente.
        if (clientfd < 0)
            continue;

        // Ahora tratamos de establecer la conexión como tal
        // si tiene éxito, podemos terminar el ciclo aquí
        if (!connect(clientfd, p->ai_addr, p->ai_addrlen))
            break;

        // Solamente llegamos aquí si falló la conexión con el socket,
        // en cuyo caso cerramos el descriptor correspondiente
        close(clientfd);
    }

    // Limpiamos la memoria que ya no vamos a usar
    freeaddrinfo(listp);

    // Devolvemos -1 si todos los intentos de conexión fallaron
    if (!p)
    {
        return -1;
    }

    // De lo contrario, el último intento de conexión antes de romper
    // el ciclo debe haber tenido éxito, así que devolvemos el descriptor
    // correspondiente.
    return clientfd;
}

int open_listenfd(char *port)
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval = 1;

    // La implementación de la función que abre el
    // socket para escuchar como servidor es muy similar, por lo
    // que solamente comentaré los lugares que son diferentes.

    memset(&hints, 0, sizeof(struct addrinfo));

    // La bandera adicional en ai_flags indica que queremos aceptar
    // conexiones en cualquier dirección IP que esté disponible
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

    // NULL como nombre porque estamos refiriéndonos a la propia máquina
    // y no a un servidor externo.
    getaddrinfo(NULL, port, &hints, &listp);

    for (p = listp; p; p = p->ai_next)
    {
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (listenfd < 0)
            continue;

        // Esto es para evitar los errores de que "la dirección ya está en uso",
        // indicando que queremos reutilizar una dirección para este socket
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        // Intentamos asociar el descriptor del socket con la dirección donde queremos
        // que reciba los mensajes
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(listenfd);
    }

    freeaddrinfo(listp);

    if (!p)
    {
        return -1;
    }
    // Nos aseguramos de que el socket está listo y dispuesto
    // a escuchar y recibir peticiones, pudiendo poner en espera como máximo LISTENQ conexiones.
    else if (listen(listenfd, LISTENQ) == 0)
    {
        return listenfd;
    }
    else
    {
        close(listenfd);
        return -1;
    }
}