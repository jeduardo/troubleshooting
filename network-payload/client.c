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
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int sockfd, n, base_size, buf_size, count;
    struct addrinfo hints, *servinfo, *p;
    char *buf_send, *buf_recv;

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port base_size\n", argv[0]);
        exit(0);
    }

    base_size = atoi(argv[3]);
    buf_size = 3 * base_size;

    printf("# Base size: %d\n", base_size);
    printf("# Payload size: %d\n", buf_size);

    buf_send = calloc(buf_size, sizeof(char));
    buf_recv = calloc(buf_size, sizeof(char));

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
    memset(buf_send, 'a', base_size);
    memset(buf_send + base_size, 'b', base_size);
    memset(buf_send + 2 * base_size, 'c', base_size);

    // Send payload
    n = write(sockfd, buf_send, buf_size);
    if (n < 0) error("ERROR writing to socket");
    printf("-> %d bytes sent\n", n);

    // Receive response
    count = 0;
    bzero(buf_recv, buf_size);

    while ((buf_size - count) > 0) {
        n = read(sockfd, buf_recv + count, buf_size);
        if (n < 0) error("ERROR reading from socket");
        printf("<- %d bytes received\n", n);
        count += n;
        if (n == 0) break;
    }

    printf("I: Total bytes received: %d\n", count);

    close(sockfd);

    printf("Send buffer content length: %ld\n", strlen(buf_send));
    printf("Recv buffer content length: %ld\n", strlen(buf_recv));

    if (strcmp(buf_send, buf_recv) == 0) {
        printf("I: Return payload is valid!\n");
        return EXIT_SUCCESS;
    } else {
        printf("I: Return payload is NOT valid\n");
    }

    return EXIT_FAILURE;
}

