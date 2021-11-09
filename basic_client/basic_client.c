#include "basic_client.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <err.h>

int create_and_connect(struct addrinfo *addrinfo)
{
    while (addrinfo != NULL)
    {
        int s = socket(addrinfo->ai_family, addrinfo->ai_socktype,
                       addrinfo->ai_protocol);
        if (s != -1)
        {
            int verif =
                connect(s, addrinfo->ai_addr, addrinfo->ai_addrlen);
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
    hints.ai_family = AF_INET;
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

int main(int argc, char **argv)
{
    if (argc <= 2 || argc >= 4)
    {
        printf("Usage: ./basic_client SERVER_IP SERVER_PORT\n");
        exit(1);
    }

    prepare_socket(argv[1], argv[2]);
    return 0;
}
