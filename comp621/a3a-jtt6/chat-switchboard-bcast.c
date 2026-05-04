/*
 * A purely event-driven chat switchboard.
 * Forwards each incoming message over all existing client connections except
 * the connection over which the message arrived.  Also supports broadcasts from
 * administrator.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "csapp.h"

#define BUF_SIZE 128 /* Per-connection internal buffer size. */

/*
 * Data structure to keep track of messages. Each message object holds one
 * complete line of message from a client.
 *
 * The message objects are maintained in a global doubly-linked list. There is a
 * dummy message head at the beginning of the list and a sentinel message at the
 * end. The sentinel is an empty message. A message is added to the list by
 * transferring the contents of the message to the existing sentinel and adding
 * the now empty message as the new sentinel.
 *
 * A message is added to the list only when a complete line has been read from
 * the client. Its reference count is set to the number of currently active
 * clients. The reference count is decremented each time the message is written
 * out to a client. The message is removed from the list and deallocated
 * (garbage collected) when its reference count reaches zero.
 */
struct msg {
	// Points to the previous message object in the doubly-linked list.
	struct msg *prev;
	
	// Points to the next message object in the doubly-linked list.
	struct msg *next;
	
	// Points to a dynamically allocated buffer holding the message.
	char *message;
	
	/*
	 * Current size of the message. This can grow if the message is read in
	 * multiple stages.
	 */
	int size;
	
	/*
	 * Reference count to keep track of number of times a message should be
	 * written out. This is decremented each time the message is written
	 * out to another client. The message object is garbage collected
	 * when the reference count reaches zero.
	 */
	int refcount;
	
	/*
	 * File descriptor of the connection on which the message arrived. This
	 * is used to ensure that we don't write the message back on that
	 * connection again.
	 */
	int fd;
};

/*
 * Data structure to keep track of client connection state.
 *
 * The connection objects are also maintained in a global doubly-linked list.
 * There is a dummy connection head at the beginning of the list.
 */
struct conn {
	// Points to the previous connection object in the doubly-linked list.
	struct conn *prev;
	
	// Points to the next connection object in the doubly-linked list.
	struct conn *next;
	
	// File descriptor associated with this connection.
	int fd;
	
	// Internal buffer to temporarily store the contents of a read.
	char buffer[BUF_SIZE];
	
	// Size of the data stored in the buffer.
	size_t size;
	
	/*
	 * Message being currently read from this connection.  This message has
	 * not been added to the doubly-linked list yet.
	 */
	struct msg *read_msg;
	
	/*
	 * This message and the ones following it on the doubly-linked list
	 * have to be written out on this connection.
	 */
	struct msg *write_msg;
	
	// Number of bytes of the current message (write_msg) written.
	int written_bytes;
};

/*
 * Data structure to keep track of active client connections.
 */
struct conn_pool { 
        /* Largest file descriptor in this pool. */
        int maxfd;

        /* Number of ready descriptors returned by select(). */
        int nready;

        /* Set of all active descriptors for reading. */
        /* Used to initialize ready_read_set.         */
        fd_set read_set;

        /* Subset of descriptors ready for reading. */
        /* Used in select().                        */
        fd_set ready_read_set;

        /* Set of all active descriptors for writing. */
        /* Used to initialize ready_write_set.        */
        fd_set write_set;

        /* Subset of descriptors ready for writing.  */
        /* Used in select().                         */
        fd_set ready_write_set;

        /* Doubly-linked list of active client connection objects. */
        struct conn *conn_head;

        /* Number of active client connections. */
        unsigned int nr_conns;

        /* Doubly-linked list of outstanding message objects. */
        struct msg *msg_head;

        /* Sentinel message of the doubly-linked list. */
        struct msg *sentinel_msg;
	
	/* Internal buffer for broadcast messages. */
	char broadcast_buffer[BUF_SIZE];
	
	/*
	 * Broadcast message being currently read from this connection.  This
	 * message has not been added to the doubly-linked list yet.
	 */
	struct msg *read_broadcast_msg;
};


/* Set verbosity to 1 for debugging. */
static int verbose = 0;

/*******************************************************************************
 * Maintaining Outstanding Messages.
 ******************************************************************************/

/*
 * Requires:
 * fd should be a valid file descriptor.
 *
 * Effects:
 * Allocates a message object and initializes it.
 */
static struct msg *
alloc_msg(int fd)
{
	struct msg *m;

	if (verbose)
		printf("Allocating message from fd %d...\n", fd);

	m = Malloc(sizeof(struct msg));
	m->size = 0;
	/* Set file descriptor of the connection on which the message arrived.
	 */
	m->fd = fd;
	return (m);
}

/*
 * Requires:
 * m should be a message object and not be NULL.
 *
 * Effects:
 * Frees the message object and the memory holding the contents of the message.
 */
static void
free_msg(struct msg *m)
{
	if (verbose)
		printf("Freeing message from fd %d...\n", m->fd);

	if (m->message != NULL)
		Free(m->message);
	Free(m);
}

/*
 * Requires:
 * m should be a message object and not be NULL.
 *
 * Effects:
 * Removes the message object from the doubly-linked list.
 */
static void
remove_msg_list(struct msg *m)
{
	m->next->prev = m->prev;
	m->prev->next = m->next;
}

/*
 * Requires:
 * m should be a message object and not be NULL.
 *
 * Effects:
 * Adds the message object to the tail of the doubly-linked list.
 */
static void
add_msg_list(struct msg *m, struct conn_pool *p)
{
	m->next = p->msg_head;
	m->prev = p->msg_head->prev;
	p->msg_head->prev->next = m;
	p->msg_head->prev = m;
}

/*
 * Requires:
 * m should be a message object and not be NULL.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * If there are no other clients, then the message is freed, and 0 is returned.
 * Otherwise, the contents of the message is transferred to the sentinel
 * message. The now empty message is added to tail of the doubly-linked list as
 * the new sentinel message.
 */
static int
finalize_msg(struct msg *m, struct conn_pool *p)
{
	if (p->nr_conns == 1 && m->fd != STDIN_FILENO) {
		if (verbose)
			printf("No other clients to send the message...\n");

		free_msg(m);
		return (0);
	}

	if (verbose) {
		printf("Finalizing message for writing from fd %d:\n", m->fd);
		printf("%s", m->message);
	}

	/* First transfer the message to the sentinel message. */
	p->sentinel_msg->message = m->message;
	p->sentinel_msg->size = m->size;
	p->sentinel_msg->refcount = p->nr_conns;
	p->sentinel_msg->fd = m->fd;

	/* Add new message as the sentinel message at the end of the list. */
	m->message = NULL;
	m->size = 0;
	m->refcount = -1;
	add_msg_list(m, p);
	p->sentinel_msg = m;

	return (1);
}

/*
 * Requires:
 * buf should not be NULL.
 * size should be the length of buf.
 *
 * Effects:
 * Scans each byte of the buffer to check for a line break. If found, its
 * position is returned. Otherwise, -1 is returned.
 */
static int
scan_buf(char *buf, int size)
{
	int i;

	for (i = 0; i < size; i++)
		if (buf[i] == '\n')
			/* Found a line break. */
			return (i);

	return (-1);
}

/*
 * Requires:
 * m should not be NULL.
 * buf should not be NULL.
 * size should be the length of buf.
 * fd should be a valid file descriptor.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Scans the buffer for line break(s) and breaks the buffer contents into one or
 * more message objects. If *m is not NULL, then the contents of the buffer
 * before the first line break are appended to this message. Each message object
 * with a complete line is finalized. If the last message object is not
 * finalized then *m is made to point to that message object. Otherwise, *m is
 * set to NULL. If one or more messages were finalized then 1 is returned.
 * Otherwise, 0 is returned.
 */
static int
update_read_msg(struct msg **m, char *buf, int size, int fd,
		struct conn_pool *p)
{
	int pos, cur_size, ret = 0;
	struct msg *tmp = *m;

	while (size != 0) {

		/* Scan the buffer to see if there is a line break. */
		pos = scan_buf(buf, size);

		if (pos != -1) {
			/*
			 * Line break found. Buffer has to be scanned again
			 * for more line breaks.
			 */
			cur_size = pos + 1;
			size = size - pos - 1;
		} else {
			/* No more line breaks. No more scanning. */
			cur_size = size;
			size = 0;
		}

		if (tmp == NULL) {
			/* Allocate a new message object. */
			tmp = alloc_msg(fd);
			/* Allocate memory for the message. */
			tmp->message = Malloc(cur_size);
		} else
			/* Reallocate memory for the updated message. */
			tmp->message =
			    Realloc(tmp->message, tmp->size + cur_size);

		/* Copy the new contents of the message. */
		memcpy(tmp->message + tmp->size, buf, cur_size);
		/* Update the size of the message. */
		tmp->size += cur_size;

		if (pos != -1) {
			/* This message hold a complete line, finalize it. */
			ret = finalize_msg(tmp, p);
			tmp = NULL;
		}

		/* Point to the location after the line break in the buffer. */
		buf += pos + 1;
		*m = tmp;
	}

	return (ret);
}

/*
 * Requires:
 * m should be a message object and not be NULL.
 *
 * Effects:
 * Decrements the reference count of a message and if the reference count is
 * zero, garbage collects it.
 */
static void
decrement_refcount_and_gc(struct msg *m)
{
	assert(m != NULL);
	
	/* FILL THIS IN. */
	m->refcount--;
	if (m->refcount == 0) {
		if (m->prev != NULL) {
			m->prev->next = m->next;
		}
		if (m->next != NULL) {
			m->next->prev = m->prev;
		}
		
		if (m->message != NULL) {
			Free(m->message);
		}

		Free(m);
	}
}

/*******************************************************************************
 * Maintaining Client Connections.
 ******************************************************************************/

/*
 * Requires:
 * c should be a connection object and not be NULL.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Adds the connection object to the tail of the doubly-linked list.
 */
static void
add_conn_list(struct conn *c, struct conn_pool *p)
{
	c->next = p->conn_head->next;
	c->prev = p->conn_head;
	p->conn_head->next->prev = c;
	p->conn_head->next = c;
}

/*
 * Requires:
 * c should be a connection object and not be NULL.
 *
 * Effects:
 * Removes the connection object from the doubly-linked list.
 */
static void
remove_conn_list(struct conn *c)
{
	c->next->prev = c->prev;
	c->prev->next = c->next;
}

/*
 * Requires:
 * c should be a connection object and not be NULL.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Closes a client connection and cleans up the associated state. Removes it
 * from the doubly-linked list and frees the connection object.
 */
static void
remove_client(struct conn *c, struct conn_pool *p)
{
	struct msg *m;

	if (verbose)
		printf("Closing connection fd %d...\n", c->fd);

	/* Close the file descriptor. */
	Close(c->fd);

	/* Remove the fd from the read and write descriptor sets. */
	FD_CLR(c->fd, &p->read_set);
	if (FD_ISSET(c->fd, &p->write_set))
		FD_CLR(c->fd, &p->write_set);

	/* If a message was being read on this connection, free it. */
	if (c->read_msg != NULL)
		free_msg(c->read_msg);

	/*
	 * If there are outstanding messages to be written on this connection,
	 * adjust their reference count.
	 */
	m = c->write_msg;
	while (m->refcount != -1) {
		decrement_refcount_and_gc(m);
		m = m->next;
	}

	/* Decrement the number of connections. */
	p->nr_conns--;

	/* Remove the connection from the list. */
	remove_conn_list(c);

	/* Free the connection object. */
	Free(c);
}

/*
 * Requires:
 * connfd should be a valid connection descriptor.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Allocates a new connection object and initializes the associated state. Adds
 * it to the doubly-linked list.
 */
static void
add_client(int connfd, struct conn_pool *p)
{
	struct conn *new_conn;

	/* Allocate a new connection object. */
	new_conn = Malloc(sizeof(struct conn));

	new_conn->fd = connfd;
	new_conn->size = 0;
	new_conn->read_msg = NULL;

	/*
	 * This points to the sentinel message so that the next actual message
	 * onward is written to this connection.
	 */
	new_conn->write_msg = p->sentinel_msg;
	new_conn->written_bytes = 0;

	/* Add this descriptor to the read descriptor set. */
	FD_SET(connfd, &p->read_set);

	/* Update max descriptor. */
	if (connfd > p->maxfd)
		p->maxfd = connfd;

	/* Update the number of client connections. */
	p->nr_conns++;

	add_conn_list(new_conn, p);
}

/*
 * Requires:
 * listenfd should be a valid listen file descriptor.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Accepts a new client connection. Sets the resulting connection file
 * descriptor to be non-blocking. Adds the client to the connection pool.
 */
static void
handle_new_connection(int listenfd, struct conn_pool *p)
{
	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(struct sockaddr_in);
	char haddrp[INET_ADDRSTRLEN];
	char host_name[NI_MAXHOST];
	int connfd, error;
	int opts = 0;

	/* Accept the new connection. */
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

	/* Set the connection descriptor to be non-blocking. */
	opts = fcntl(connfd, F_GETFL);
	if (opts < 0) {
		printf("fcntl error.");
		exit(-1);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(connfd, F_SETFL, opts) < 0) {
		printf("fcntl set error.");
		exit(-1);
	}

	if (verbose) {
		/* determine the domain name and IP address of the client */
		error = getnameinfo((struct sockaddr *)&clientaddr,
				    sizeof(clientaddr), host_name,
				    sizeof(host_name), NULL, 0, 0);
		if (error != 0) {
			fprintf(stderr, "ERROR: %s\n", gai_strerror(error));
			Close(connfd);
		}
		inet_ntop(AF_INET, &clientaddr.sin_addr, haddrp,
			  INET_ADDRSTRLEN);
		printf("Accepted new connection request from %s (%s) new fd "
		       "%d...\n",
		       host_name, haddrp, connfd);
	}

	/* Create new connection object and add it to the connection pool. */
	add_client(connfd, p);
	p->nready--;
}

/*
 * Requires:
 * listenfd should be a valid listen file descriptor.
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Initializes an empty connection pool. Allocates and initializes dummy list
 * heads.
 */
static void
init_pool(int listenfd, struct conn_pool *p)
{
	/* Initially, there are no connected descriptors. */
	p->nr_conns = 0;

	/* Allocate and initialize the dummy message head. */
	p->msg_head = Malloc(sizeof(struct msg));
	p->msg_head->next = p->msg_head;
	p->msg_head->prev = p->msg_head;

	/* Allocate and initialize the sentinel message. */
	p->sentinel_msg = Malloc(sizeof(struct msg));
	p->sentinel_msg->refcount = -1;
	p->sentinel_msg->message = NULL;
	/* Add it to the list. */
	add_msg_list(p->sentinel_msg, p);

	/* Allocate and initialize the dummy connection head. */
	p->conn_head = Malloc(sizeof(struct conn));
	p->conn_head->next = p->conn_head;
	p->conn_head->prev = p->conn_head;

	// p->read_broadcast_msg = NULL;
	p->read_broadcast_msg = Malloc(sizeof(struct msg));
	p->read_broadcast_msg->refcount = -1;
	p->read_broadcast_msg->message = NULL;
	p->read_broadcast_msg->size = 0;
	p->read_broadcast_msg->fd = STDIN_FILENO;
	/*
	 * Initially, listenfd and STDIN are the only member of the read
	 * descriptor set.
	 */
	p->maxfd = listenfd;
	FD_ZERO(&p->read_set);
	FD_SET(listenfd, &p->read_set);
	/* FILL THIS IN. */
	FD_SET(STDIN_FILENO, &p->read_set);
	if (STDIN_FILENO > p->maxfd) {
		p->maxfd = STDIN_FILENO;
	}

	/* Initially, the write descriptor set is empty. */
	FD_ZERO(&p->write_set);

	FD_ZERO(&p->ready_read_set);
    FD_ZERO(&p->ready_write_set);

    /* Clear the broadcast buffer. */
    memset(p->broadcast_buffer, 0, BUF_SIZE);
}

/*******************************************************************************
 * Read and Write Messages.
 ******************************************************************************/

/*
 * Requires:
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Sets all file descriptors in the write set.
 */
static void
set_all_write_fds(struct conn_pool *p)
{
	struct conn *c;

	if (verbose)
		printf("Setting all descriptors in write set...\n");

	for (c = p->conn_head->next; c != p->conn_head; c = c->next)
		FD_SET(c->fd, &p->write_set);
}

void
read_broadcast_messages(struct conn_pool *p)
{
	// int n = 0;

	/* Read from STDIN. */
	/* FILL THIS IN. */
	// p = (struct conn_pool *)p;
	// n = (int)n;
	int n = read(STDIN_FILENO, p->broadcast_buffer, BUF_SIZE);
	if (verbose) {
		printf("num bytes of broadcast: %d\n", n);
	}

	/*
	 * If one or more bytes of a message was read call update_read_msg().
	 * Else print error messages.
	 */
	/* FILL THIS IN. */
	if (n > 0) {
		if (update_read_msg(&p->read_broadcast_msg, p->broadcast_buffer,
			n, STDIN_FILENO, p)) {
				if (verbose)
                	printf("Broadcast message finalized. Resetting client write pointers...\n");

				// Reset write_msg for all clients who were caught up
				// Reset write_msg for all clients to point to the new broadcast message
				struct conn *c;
				for (c = p->conn_head->next; c != p->conn_head; c = c->next) {
					c->write_msg = p->sentinel_msg->prev;  // Point to the finalized broadcast
					c->written_bytes = 0;
				}

				// Mark all clients as ready to write
				set_all_write_fds(p);

				// Prepare a new empty message for the next broadcast
				p->read_broadcast_msg = Malloc(sizeof(struct msg));
				p->read_broadcast_msg->refcount = -1;
				p->read_broadcast_msg->message = NULL;
				p->read_broadcast_msg->size = 0;
				p->read_broadcast_msg->fd = STDIN_FILENO;
		} else if (n < 0) {
			if (errno != EAGAIN && errno != EINTR)
				printf("reading broadcast message error\n");
		}	
	}
}

/*
 * Requires:
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Reads from each ready file descriptor in the read set and handles the
 * incoming messages appropriately.
 */
static void
read_messages(struct conn_pool *p)
{
	int n;
	struct conn *c, *next;

	/*
	 * Scan all connections to check if the corresponding file descriptor is
	 * on the ready set.
	 */
	for (c = p->conn_head->next; c != p->conn_head; c = next) {

		next = c->next;

		/* If the descriptor is ready to be read, do it. */
		if (FD_ISSET(c->fd, &p->ready_read_set)) {
			assert(p->nready != 0);
			p->nready--;

			/* Read from that socket. */
			n = recv(c->fd, c->buffer, BUF_SIZE, 0);

			/* Data read. */
			if (n > 0) {

				if (verbose) {
					c->buffer[n] = '\0';
					printf("Read %d bytes from fd %d:\n", n,
					       c->fd);
					printf("%s\n", c->buffer);
				}

				if (update_read_msg(&c->read_msg, c->buffer, n,
						    c->fd, p))
					set_all_write_fds(p);
			}
			/* Error (possibly). */
			else if (n < 0) {
				/* If errno is EAGAIN, it just means we need to
				 * read again. */
				if (errno != EAGAIN)
					remove_client(c, p);
			}
			/* Connection closed by client. */
			else
				remove_client(c, p);
		}
	}
}

/*
 * Requires:
 * p should be a connection pool and not be NULL.
 *
 * Effects:
 * Writes the appropriate messages to each ready file descriptor in the write
 * set.
 */
static void
write_messages(struct conn_pool *p)
{
	int n;
	struct conn *c, *next = NULL;
	struct msg *wmsg;

	/*
	 * Scan all connections to check if the corresponding file descriptor is
	 * on the ready set.
	 */
	for (c = p->conn_head->next; /* FILL THIS IN */c != p->conn_head; c = next) {

		/* EDIT THIS to get the next connection. */
		/* Dummy statement to avoid compilation errors. */
		// next = (struct conn *)next;
		next = c->next;

		/* If the descriptor is ready for a write, do it. */
		if (/* FILL THIS IN. */ FD_ISSET(c->fd, &p->ready_write_set) != 0) {
			assert(p->nready != 0);
			p->nready--;

			wmsg = c->write_msg;

			/*
			 * If write_msg is pointing to the sentinel message,
			 * then there
			 * is nothing to write.	 If there is nothing to write,
			 * this file descriptor should never have been returned
			 * by select.
			 */
			assert(wmsg->refcount != -1);

			/* Skip the write if the message arrived over this
			 * connection. */
			if (/* FILL THIS IN. */ wmsg->fd == c->fd) {

				/* Find the next message to write. */
				c->write_msg = c->write_msg->next;
				if (c->write_msg->refcount == -1) {
					/*
					 * Nothing to write. Clear this
					 * connection descriptor from the write
					 * set.
					 */
					/* FILL THIS IN. */
					FD_CLR(c->fd, &p->write_set);
				}

				decrement_refcount_and_gc(wmsg);
				continue;
			}

			/* Perform the write system call. */
			/* FILL THIS IN. */;
			n = write(c->fd, 
				wmsg->message + c->written_bytes, 
				wmsg->size - c->written_bytes);

			/* Data written. */
			if (n > 0) {

				/* Update the bytes written count. */
				c->written_bytes += n;
				/* Check if entire msg has been written on this
				 * connection. */
				if (c->written_bytes == wmsg->size) {

					if (verbose) {
						printf("Wrote %d bytes to fd "
						       "%d:\n",
						       (int)wmsg->size, c->fd);
						printf("%s", wmsg->message);
					}

					c->written_bytes = 0;

					/* Find the next message to write. */
					/* FILL THIS IN. */
					c->write_msg = c->write_msg->next;

					if (c->write_msg->refcount == -1) {
						/*
						 * Nothing to write. Clear this
						 * connection descriptor from
						 * the write set.
						 */
						/* FILL THIS IN. */
						FD_CLR(c->fd, &p->write_set);
					}

					decrement_refcount_and_gc(wmsg);
				}
			}
			/* Error (possibly). */
			else if (n < 0) {
				/* If errno is EAGAIN, it just means we have to
				 * write again. */
				if (errno != EAGAIN)
					remove_client(c, p);
			}
			/* Connection closed by client. */
			else
				remove_client(c, p);
		}
	}
}

/*
 * Requires:
 * None.
 *
 * Effects:
 * Sets up a listen socket at the given port.  Then uses select to wait for
 * events such as new connections requests, sockets with data to be read, and
 * sockets which are ready to be written.
 */
int
main(int argc, char **argv)
{
	int listenfd;
	struct conn_pool pool;

	if (verbose)
		printf("Starting chat switchboard...\n");

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	/* Create a listen socket. */
	listenfd = Open_listenfd(argv[1]);

	/* Initialize the connection pool. */
	init_pool(listenfd, &pool);

	if (verbose)
		printf("Listening for new connections at port %s...\n",
		       argv[1]);

	while (true) {
                /*
                 * Listen for connection requests by anything in the connection pool.
                 * Handle any new connection request.
                 * Write to any ready write FDs.
                 * Read from any ready read FDs. 
                 */ 
		/* FILL THIS IN. */
		if (verbose) {
			printf("entered while loop\n");
		}
		pool.ready_read_set = pool.read_set;
		pool.ready_write_set = pool.write_set;
		
		if (verbose) {
			printf("before select call\n");
			printf("maxfd = %d, listenfd = %d, STDIN = %d\n", pool.maxfd, listenfd, STDIN_FILENO);
		}
		
		pool.nready = select(pool.maxfd + 1, &pool.ready_read_set, &pool.ready_write_set, NULL, NULL);
		if (verbose) {
			printf("past select call\n");
		}
		if (pool.nready < 0) {
			printf("select() error\n");
		}
		
		if (FD_ISSET(listenfd, &(pool.ready_read_set))) {
			handle_new_connection(listenfd, &pool);
		}

		if (verbose) {
			printf("right before call to read_broadcast_messages in main\n");
		}
		if (FD_ISSET(STDIN_FILENO, &(pool.ready_read_set))) {
			if (verbose) {
				printf("about to call read_broadcast_messages in main\n");
			}
			read_broadcast_messages(&pool);
		}
		
		write_messages(&pool);
		read_messages(&pool);
	}

	/* Dummy call to have the program compile. */
	remove_msg_list(NULL);
}
