#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, n, payload_size, buf_size;
    struct addrinfo hints, *servinfo, *p;
    char *buffer;

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port payload_size\n", argv[0]);
        exit(0);
    }

    payload_size = atoi(argv[3]);
    buf_size = 3 * payload_size;

    printf("# Payload size chosen: %d\n", payload_size);
    printf("# Buffer size calculated: %d\n", buf_size);

    buffer = calloc(buf_size, sizeof(char));

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // anny addr
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], argv[2], &hints, &servinfo) != 0)
        error("getaddrinfo");

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;  // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return EXIT_FAILURE;
    }

    freeaddrinfo(servinfo);

    // Prepare payload
    memset(buffer, 'a', payload_size);
    memset(buffer + payload_size, 'b', payload_size);
    memset(buffer + 2 * payload_size, 'c', payload_size);

    // Send payload
    n = write(sockfd, buffer, buf_size);
    if (n < 0) error("ERROR writing to socket");
    printf("-> %d bytes sent\n", n);

    // Receive response
    bzero(buffer, buf_size);
    n = read(sockfd, buffer, buf_size);
    if (n < 0) error("ERROR reading from socket");
    printf("Received: %s\n", buffer);
    printf("<- %d bytes received\n", n);

    close(sockfd);

    return 0;
}

