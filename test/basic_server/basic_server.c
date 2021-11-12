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

void communicate(int client_socket)
{
    char *buffer = malloc(DEFAULT_BUFFER_SIZE + 1);
    buffer[DEFAULT_BUFFER_SIZE] = '\0';
    ssize_t received;
    ssize_t pos = 0;
    ssize_t len = 1;
    char *stack = malloc(len);
    while ((received = recv(client_socket, buffer, DEFAULT_BUFFER_SIZE, 0)) != 0)
    {
        // printf("%ld %s", received, buffer);
        if (received == -1)
            errx(1, "basic_server: recv returned -1");
        buffer[received] = '\0';


        if (pos + received < len)
        {
            strcpy((stack + pos), buffer);
            pos += received;
        }
        else
        {
            stack = realloc(stack, len + received);
            len += received;
            strcpy((stack + pos), buffer);
            pos += received;
        }
        if (buffer[received - 1] == '\n')
        {
            stack[pos] = '\0';
            printf("Received body: %s", stack);
            while (pos > 0)
            {
                ssize_t sent = 0;
                if (pos > DEFAULT_BUFFER_SIZE)
                {
                    sent = send(client_socket, stack, (DEFAULT_BUFFER_SIZE - 1), MSG_NOSIGNAL);
                    if (sent == -1)
                        break;
                    pos -= (DEFAULT_BUFFER_SIZE - 1);
                }
                else
                {
                    sent = send(client_socket, stack, pos, MSG_NOSIGNAL);
                    if (sent == -1)
                        break;
                    pos = 0;
                }
            }
        }
        else
        {
            printf("NO\n");
        }
    }
    free(stack);
    free(buffer);
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
        while (1)
        {
            if (verif == 0)
            {
                int client_socket = accept_client(socket);
                if (client_socket != -1)
                {
                    printf("Client connected\n");
                    communicate(client_socket);
                    printf("Client disconnected\n");
                    close(client_socket);
                }
                else
                {
                    freeaddrinfo(servinfo);
                    errx(1, "basic_server: error, could not accept client");
                }
            }
        }
        freeaddrinfo(servinfo);
        return socket;
    }
    freeaddrinfo(servinfo);
    errx(1, "basic_server: error in prepare_socket");
}

int main(int argc, char **argv)
{
    if (argc <= 2 || argc >= 4)
    {
        fprintf(stderr, "Usage: ./basic_server SERVER_IP SERVER_PORT\n");
        exit(1);
    }

    int socket = prepare_socket(argv[1], argv[2]);
    close(socket);
    return 0;
}
