#define _POSIX_C_SOURCE 200112L

#include "basic_client.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_and_connect(struct addrinfo *addrinfo)
{
    while (addrinfo != NULL)
    {
        int s = socket(addrinfo->ai_family, addrinfo->ai_socktype, IPPROTO_TCP);
        if (s != -1)
        {
            int verif = connect(s, addrinfo->ai_addr, addrinfo->ai_addrlen);
            // printf("%d\n", verif);
            if (verif != -1)
            {
                return s;
            }
        }
        addrinfo = addrinfo->ai_next;
    }
    errx(1, "basic_client: error in create_and_connect");
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
        socket = create_and_connect(servinfo);
        freeaddrinfo(servinfo);
        return socket;
    }
    else
    {
        freeaddrinfo(servinfo);
        errx(1, "basic_client: error in prepare_socket");
    }
}

void communicate(int server_socket)
{
    char buffer[DEFAULT_BUFFER_SIZE];
    // int count_print = 0;
    fprintf(stderr, "Enter your message:\n");
    while (fgets(buffer, DEFAULT_BUFFER_SIZE, stdin))
    {
        if (buffer[0] == '\0')
            continue;

        int len = strlen(buffer);
        printf("%d\n", len);
        if (len > 0)
        {
            ssize_t sent = send(server_socket, buffer, len, MSG_NOSIGNAL);
            if (sent == -1)
                errx(1, "basic_client: error in communicate send");
            if (buffer[len-1] != '\n')
                continue;
        }
        ssize_t received = 0;
        printf("Server answered with: ");
        while ((received =
        recv(server_socket, buffer, DEFAULT_BUFFER_SIZE - 1, 0)) > 0 && buffer[received - 1] != '\n')
        {
            buffer[received] = '\0';
            printf("%s", buffer);
        }
        printf("%s", buffer);
        fprintf(stderr, "Enter your message:\n");
    }
}

int main(int argc, char **argv)
{
    if (argc <= 2 || argc >= 4)
    {
        fprintf(stderr, "Usage: ./basic_client SERVER_IP SERVER_PORT\n");
        exit(1);
    }

    int socket = prepare_socket(argv[1], argv[2]);
    communicate(socket);
    close(socket);
    return 0;
}
