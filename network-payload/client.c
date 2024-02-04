#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, n, base_size, buf_size;
    struct addrinfo hints, *servinfo, *p;
    char *buffer;

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port base_size\n", argv[0]);
        exit(0);
    }

    base_size = atoi(argv[3]);
    buf_size = 3 * base_size;

    printf("# Base size: %d\n", base_size);
    printf("# Payload size: %d\n", buf_size);

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
    memset(buffer, 'a', base_size);
    memset(buffer + base_size, 'b', base_size);
    memset(buffer + 2 * base_size, 'c', base_size);

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

