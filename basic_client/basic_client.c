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
        // printf("%d\n", socket);
        return socket;
    }
    else
    {
        errx(1, "basic_client: error in prepare_socket");
    }
}

void communicate(int server_socket)
{
    char buffer[DEFAULT_BUFFER_SIZE];
    char *buff;
    char *buffer2;
    int count_print = 0;
    fprintf(stderr, "Enter your message:\n");
    while (fgets(buffer, DEFAULT_BUFFER_SIZE - 1, stdin))
    {
        int len = strlen(buffer);
        // printf("%d\n", len);
        buff = malloc(sizeof(char) * len + 1);
        buff = strcpy(buff, buffer);
        ssize_t sent = send(server_socket, buff, len, MSG_NOSIGNAL);
        if (sent == -1)
            errx(1, "basic_client: error in communicate send");

        buffer2 = malloc(sizeof(char) * len + 1);
        ssize_t received = recv(server_socket, buffer2, len, 0);
        if (received == -1)
            errx(1, "basic_client: error in communicate recv");

        if (received < len)
            buffer2[received] = '\0';
        else
            buffer2[len] = '\0';

        if (count_print == 0)
            printf("Server answered with: %s", buffer2);
        else
            printf("%s", buffer2);
        if (len >= DEFAULT_BUFFER_SIZE - 2)
        {
            if (count_print == 0)
                count_print = 1;
        }
        else
        {
            // fprintf(stderr, "Enter your message:\n");
            count_print = 0;
        }
        free(buff);
        free(buffer2);
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
