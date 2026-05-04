/*
 * Requires:
 *   "connfd" is a connected socket.
 *
 * Effects:
 *   Echoes lines of text until the client closes the connection.
 */
void	echo(int connfd);
