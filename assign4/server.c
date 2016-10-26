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

#include "game.c"

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
    serverAddr.sin_family = AF_INET;    // IP v4
    serverAddr.sin_port = htons(port);  // port num
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // any IP address of this host

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

    Game *game = new_game();

    char buffer[1024];
    ssize_t numBytesRead;

    // Obtain connected file descriptor from argument passed in
    int fd = *(int *)arg;
    free(arg);

    // Repeatedly read data arriving from client - turn it to upper case - 
    // send it back to client
    while((numBytesRead = read(fd, buffer, 1024)) > 0) {
        char *response = process_message(game, buffer);
        fprintf(stderr, "%s", response);
        write(fd, response, strlen(response) + 1);
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
