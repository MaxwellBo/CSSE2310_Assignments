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

void send_HTTP_request(int fd, char* file, char* host) {
    char* requestString;

    requestString = (char*)malloc(strlen(file) + strlen(host) + 26);

    sprintf(requestString, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", file, host);

    if(write(fd, requestString, strlen(requestString)) < 1) {
        perror("Write error");
        exit(1);
    }
}

void get_and_output_HTTP_response(int fd) {
    char buffer[1024];
    int numBytesRead;
    int eof = 0;

    while(!eof) {
        numBytesRead = read(fd, buffer, 1024);
        if(numBytesRead < 0) {
            perror("Read error\n");
            exit(1);
        } else if(numBytesRead == 0) {
            eof = 1;
        } else {
            fwrite(buffer, sizeof(char), numBytesRead, stdout);
        }
    }
}

