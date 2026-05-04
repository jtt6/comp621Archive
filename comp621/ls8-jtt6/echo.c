#include "csapp.h"
#include "echo.h"

/*
 * Requires:
 *   "connfd" is a connected socket.
 *
 * Effects:
 *   Echoes lines of text until the client closes the connection.
 */
void
echo(int connfd)
{
	rio_t rio;
	size_t n; 
	char buf[MAXLINE]; 

	Rio_readinitb(&rio, connfd);
	while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		printf("server received %zu bytes\n", n);
		Rio_writen(connfd, buf, n);
	}
}
