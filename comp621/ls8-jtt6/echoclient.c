/*
 * This file implements an echo client.
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csapp.h"
#include <arpa/inet.h>
static int	open_client(char *hostname, int port);

/*
 * Requires:
 *   argv[1] is a string representing a host name, and argv[2] is a string
 *   representing a TCP port number (in decimal).
 *
 * Effects:
 *   Opens a connection to the specified server.  Then, repeats the following
 *   actions.  First, reads a line from stdin, and writes that line to the
 *   server.  Second, reads a line from the server, and writes that line to
 *   stdout.  This loop stops when reading a line from stdin returns EOF. 
 */
int
main(int argc, char **argv)
{
	rio_t rio;
	int clientfd, port;
	char *host, buf[MAXLINE];

	if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	port = atoi(argv[2]);
	clientfd = open_client(host, port);
	if (clientfd == -1) {
		unix_error("open_clientfd Unix error");
	} else if (clientfd == -2) {
		dns_error("open_clientfd DNS error");
	}
	Rio_readinitb(&rio, clientfd);
	while (Fgets(buf, MAXLINE, stdin) != NULL) {
		Rio_writen(clientfd, buf, strlen(buf));
		Rio_readlineb(&rio, buf, MAXLINE);
		Fputs(buf, stdout);
	}
	Close(clientfd);
	return (0);
}

/*
 * Requires:
 *   hostname points to a string representing a host name, and port in an
 *   integer representing a TCP port number.
 *
 * Effects:
 *   Opens a TCP connection to the server at <hostname, port> and returns a
 *   file descriptor ready for reading and writing.  Returns -1 and sets
 *   errno on a Unix error.  Returns -2 on a DNS (getaddrinfo) error.
 */
static int
open_client(char *hostname, int port)
{
	struct sockaddr_in serveraddr;
	struct addrinfo *ai;
	int clientfd;

	/*
	 * Prevent "unused parameter/variable" warnings.  REMOVE THESE
	 * STATEMENTS!
	 */
	// (void)hostname;
	// (void)port;
	// (void)ai;

	// Set clientfd to a newly created stream socket.
	// REPLACE THIS.
	clientfd = socket(AF_INET, SOCK_STREAM, 0);

	// Use getaddrinfo() to get the server's IP address.
	// FILL THIS IN.
	if (getaddrinfo(hostname, NULL, NULL, &ai) != 0) {
		printf("getaddrinfo error");
	}

	/*
	 * Set the address of serveraddr to be server's IP address and port.
	 * Be careful to ensure that the IP address and port are in network
	 * byte order.
	 */
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	// COMPLETE THIS.
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr = ((struct sockaddr_in *) (ai->ai_addr))->sin_addr;


	// Establish a connection to the server with connect().
	// FILL THIS IN.
	connect(clientfd, ai->ai_addr, ai->ai_addrlen);

	return (clientfd);
}
