#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*int sockfd = socket(domain, type, protocol)
 * specifique dans server.c
-> je sais pas si necessaire : int setsockopt(sockfd, int level, int optname, const void *optval, sockleb_t optlen)
-> int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) ->binds the socket to the address specified in addr
-> int listen(int sockfd, int backlog) -> met le serveur en passif et attends que le client essaye de s'y connecter
->int new_socket = accept(int sockfd, struct sockaddr *addr, socklen)t addrlen); -> extracts the first connection request on the queue of pending
connections for the listenning connection , creates a new socket and returns the file descriptor

specifique a client.c 
-> connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)

sous fonction get addrinfo pour avoir les infos necessaires pour bind le socket et tout
faut pas oublier de freeaddrinfo a un moment

chaque message sera dleimite par un '\n' !, le server repondra aussi avec le meme protocol
et donc un newline a la fin de son message !*/

int create_and_connect(struct addrinfo *addrinfo)
{
	while (addrinfo != NULL)
	{
		int s = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
		if (s != -1)
		{
			int verif = connect(s, addrinfo->ai_addr, sizeof(addrinfo->ai_addrlen));
			if (verif != -1)
			{
				return s;
			}
		}
		addrinfo = addrinfo->ai_next;
	}
	exit(1);
	return 1;
}

int main(int argc, char **argv)
{
	if (argc <= 2 || argc >= 4)
	{
		printf("Usage: ./basic_client SERVER_IP SERVER_PORT\n");
		exit(1);
	}
	int status;
	struct addrinfo *servinfo = NULL;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(argv[1], argv[2], &hints, &servinfo);
	if (status != 0)
	{
		exit(1);
	}
	int socket = create_and_connect(servinfo);
}
