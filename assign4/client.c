#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <netdb.h>
#include <string.h>

struct in_addr* name_to_IP_addr(char* hostname) {
    int error;
    struct addrinfo* addressInfo;

    error = getaddrinfo(hostname, NULL, NULL, &addressInfo);
    if(error) {
        return NULL;
    }
    return &(((struct sockaddr_in*)(addressInfo->ai_addr))->sin_addr);
}

int connect_to(struct in_addr* ipAddress, int port) {
    struct sockaddr_in socketAddr;
    int fd;
    
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(port);
    socketAddr.sin_addr.s_addr = ipAddress->s_addr;

    if(connect(fd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("Error connecting");
        exit(1);
    }

    return fd;
}

void process_connection(int fd) {
    Game *game = new_game();

    char greeting[128];
    sprintf(greeting, "fightmeirl %s\n", game->team->name);
    write(fd, greeting, strlen(greeting) + 1);

    char buffer[1024];
    ssize_t numBytesRead;

    while((numBytesRead = read(fd, buffer, 1024)) > 0) {
        char *response = process_message(game, buffer);
        write(fd, response, strlen(response) + 1);
    }
    // EOF - client disconnected

    if(numBytesRead < 0) {
        // perror("Error reading from socket");
        exit(1);
    }

    add_elimination_narrative_line(game, game->theirName);
    // Print a message to server's stdout, on disconnect
    print_narrative(game); 

    close(fd);
}



