#define _POSIX_C_SOURCE 200112L

#include "basic_server.h"

#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_and_bind(struct addrinfo *addrinfo)
{
    while (addrinfo != NULL)
    {
        int s = socket(addrinfo->ai_family, addrinfo->ai_socktype, IPPROTO_TCP);
        int optval = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
        if (s != -1)
        {
            int bind_v = bind(s, addrinfo->ai_addr, addrinfo->ai_addrlen);
            // fprintf(stderr, "%d %d %d\n", s, bind_v, errno);
            if (bind_v != -1)
            {
                return s;
            }
        }
        addrinfo = addrinfo->ai_next;
    }
    errx(1, "basic_server: error in create_and_bind");
}

int accept_client(int socket)
{
    return accept(socket, NULL, NULL);
}

int prepare_socket(const char *ip, const char *port)
{
    int status;
    struct addrinfo *servinfo = NULL;
    struct addrinfo hints;
    int socket;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip, port, &hints, &servinfo)) == 0)
    {
        socket = create_and_bind(servinfo);
        int verif = listen(socket, 10);
        if (verif == 0)
        {
            socket = accept_client(socket);
            if (socket != -1)
            {
                printf("Client connected\n");
                freeaddrinfo(servinfo);
                return socket;
            }
            else
            {
                freeaddrinfo(servinfo);
                errx(1, "basic_server: error, could not accept client");
            }
        }
    }
    freeaddrinfo(servinfo);
    errx(1, "basic_server: error in prepare_socket");
}

void communicate(int client_socket)
{
    char buffer[DEFAULT_BUFFER_SIZE];
    ssize_t received;
    while((received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE, 0)))
    {
        if (received == -1)
            errx(1, "basic_server: recv returned -1");
        printf("Received Body: %s", buffer);
        int len = strlen(buffer);
        ssize_t sent = send(client_socket, buffer, len, MSG_NOSIGNAL);
        if (sent == -1)
            errx(1, "basic_server: error in communicate send");
    }
}

int main(int argc, char **argv)
{
    if (argc <= 2 || argc >= 4)
    {
        fprintf(stderr, "Usage: ./basic_server SERVER_IP SERVER_PORT\n");
        exit(1);
    }

    int socket = prepare_socket(argv[1], argv[2]);
    communicate(socket);
    close(socket);
    printf("Client disconnected\n");
    return 0;
}
