#define _POSIX_C_SOURCE 200112L

#include "basic_server.h"

#include <err.h>
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
        if (s != -1)
        {
            if (bind(s, addrinfo->ai_addr, addrinfo->ai_addrlen) >= 0)
            {
                return s;
            }
        }
        addrinfo = addrinfo->ai_next;
    }
    errx(1, "basic_server: error in create_and_connect");
}

int accept_client(int socket)
{
    int new_socket = accept(socket, NULL, NULL);
    if (socket != -1)
    {
        return new_socket;
    }
    else
    {
        return -1;
    }
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
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(ip, port, &hints, &servinfo)) == 0)
    {
        socket = create_and_bind(servinfo);
        if (socket >= 0)
        {
            int verif = listen(socket, 6);
            if (verif == 0)
            {
                socket = accept_client(socket);
                if (socket >= 0)
                {
                    printf("Client connected\n");
                    return socket;
                }
                else
                {
                    errx(1, "basic_server: error, could not accept client");
                }
            }
        }
        freeaddrinfo(servinfo);
    }
    errx(1, "basic_server: error in prepare_socket");
}

void communicate(int client_socket)
{
    if (client_socket != -1)
    {
        return;
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
    return 0;
}
