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
    int count_print = 0;
    fprintf(stderr, "Enter your message:\n");
    while (fgets(buffer, DEFAULT_BUFFER_SIZE - 1, stdin))
    {
        if (buffer[0] == '\0')
            continue;
        int len = strlen(buffer);
        // printf("%d\n", len);
        // char buff[len + 1];
        // for (int i = 0; i < len + 1; i++)
        //     buff[i] = buffer[i];
        // buff[len] = '\0';
        ssize_t sent = send(server_socket, buffer, len, MSG_NOSIGNAL);
        if (sent == -1)
            errx(1, "basic_client: error in communicate send");

        // char buffer2[len + 1];
        // buffer2[len] = '\0';
        ssize_t received = recv(server_socket, buffer, len, 0);
        if (received == -1)
            errx(1, "basic_client: error in communicate recv");

        if (count_print == 0)
            printf("Server answered with: %s", buffer);
        else
            printf("%s", buffer);
        if (len >= DEFAULT_BUFFER_SIZE - 2)
        {
            if (count_print == 0)
                count_print = 1;
        }
        else
        {
            fprintf(stderr, "Enter your message:\n");
            count_print = 0;
        }
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
