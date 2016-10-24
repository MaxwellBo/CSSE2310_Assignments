#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "utils.c"

#define CONTROLLERPORT 1
#define COMMAND 1
#define TEAMFILE 2
#define SINISTERFILE 3
#define TARGETPORT 4

#define MIN_ARGS 3
#define SIMULATION_ARGS 3

#define WAIT_ARGS 4

#define CHALLENGE_ARGS 5
#define MAX_ARGS 5

#define MAXHOSTNAMELEN 128

void* client_thread(void* arg);

int open_listen(int port)
{
    int fd;
    struct sockaddr_in serverAddr;
    int optVal;

    // Create socket (TCP IPv4)
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Allow address (port number) to be reused immediately
    optVal = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(int)) < 0) {
        perror("Error setting socket option");
        exit(1);
    }

    // Populate server address structure to indicate local address our
    // server is going to listen on
    serverAddr.sin_family = AF_INET;	// IP v4
    serverAddr.sin_port = htons(port);	// port num
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// any IP address of this host

    // Bind our socket to address we just created
    if(bind(fd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)) < 0) {
        perror("Error binding socket to port");
        exit(1);
    }

    // Indicate our willingness to accept connections. SOMAXCONN (128 default) - length
    // of queue of connections waiting to be accepted
    if(listen(fd, SOMAXCONN) < 0) {
        perror("Error listening");
        exit(1);
    }

    return fd;
}

void process_connections(int fdServer)
{
    int fd;
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize;
    int error;
    char hostname[MAXHOSTNAMELEN];
    pthread_t threadId;

    // Repeatedly accept connections and process data (capitalise)
    while(1) {
        fromAddrSize = sizeof(struct sockaddr_in);
	// Block, waiting for a new connection. (fromAddr will be populated
	// with address of client)
        fd = accept(fdServer, (struct sockaddr*)&fromAddr,  &fromAddrSize);
        if(fd < 0) {
            perror("Error accepting connection");
            exit(1);
        }
     
	// Turn our client address into a hostname and print out both the address
	// and hostname as well as the port number
        error = getnameinfo((struct sockaddr*)&fromAddr, fromAddrSize, hostname,
                MAXHOSTNAMELEN, NULL, 0, 0);
        if(error) {
            fprintf(stderr, "Error getting hostname: %s\n", 
                    gai_strerror(error));
        } else {
            printf("Accepted connection from %s (%s), port %d\n", 
                    inet_ntoa(fromAddr.sin_addr), hostname,
                    ntohs(fromAddr.sin_port));
        }

	// Start new thread to deal with client communication
	int* fdPtr = malloc(sizeof(int));
	*fdPtr = fd;
	pthread_create(&threadId, NULL, client_thread, fdPtr);
	pthread_detach(threadId);
    }
}

void* client_thread(void* arg) {
    char buffer[1024];
    ssize_t numBytesRead;

    // Obtain connected file descriptor from argument passed in
    int fd = *(int *)arg;
    free(arg);

    // Repeatedly read data arriving from client - turn it to upper case - 
    // send it back to client
    while((numBytesRead = read(fd, buffer, 1024)) > 0) {
    	// capitalise(buffer, numBytesRead);
    	write(fd, buffer, numBytesRead);
    }
    // EOF - client disconnected

    if(numBytesRead < 0) {
    	perror("Error reading from socket");
    	exit(1);
    }
    // Print a message to server's stdout
    printf("Done\n");
    fflush(stdout);
    close(fd);

    // Could have pthread_exit(NULL);
    return NULL;
}

/**
 * Takes the argument count and argument array, and checks if they're valid.
 *
 * - Terminates the program if the program if invalid arguments are provided,
 *   and prints an appropriate error message to stderr
 */
void validate_args(int argc, char **argv) {
    int status = 0;

    // if not within the valid range
    if (!((MIN_ARGS <= argc && argc <= MAX_ARGS))) {
        status = 1;
    } else if (argc == SIMULATION_ARGS && atoi(argv[CONTROLLERPORT]) < 1 ) {
        status = 6;
    } else if (argc == CHALLENGE_ARGS && atoi(argv[TARGETPORT]) < 1 ) {
        status = 6;
    }

    if (status) {
        fprintf(stderr, "%s\n", get_error_message_2310team(status)); 
        exit(status); 
    }

    return;
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
    validate_args(argc, argv);

    if (argc == SIMULATION_ARGS) {
    } else if (argc == WAIT_ARGS) {
        // Team *team = read_teamfile(argv[TEAMFILE]);
    } else if (argc == CHALLENGE_ARGS) {
        // Team *team = read_teamfile(argv[TEAMFILE]);
        // read_sinisterfile(argv[SINISTERFILE]);
    }

    // TODO: ephemeral port
    int port = 2000;

    fprintf(stdout, "%d\n", port);
    fflush(stdout);

    int fdServer = open_listen(port);
    process_connections(fdServer);

    return 0;
}
