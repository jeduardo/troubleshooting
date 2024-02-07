#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
    int sockfd, n, base_size, buf_size, count, ret, mss = 0;
    socklen_t len;
    struct addrinfo hints, *servinfo, *p;
    char *buf_send, *buf_recv;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s hostname port base_size [mss]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc > 4) mss = atoi(argv[4]);

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

        if (mss > 0)
            if (setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)) <
                0)
                error("setsockopt TCP_MAXSEG failed");

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

    len = sizeof(mss);
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len) < 0)
        error("getsockopt TCP_MAXSEG failed");
    printf("# MSS is %d\n", mss);

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

    printf("# Total bytes received: %d\n", count);

    close(sockfd);

    ret = EXIT_SUCCESS;
    for (count = 0; count < buf_size; count++) {
        if (buf_send[count] != buf_recv[count]) {
            printf("E: Received payload different on byte %d\n", count);
            ret = EXIT_FAILURE;
            break;
        }
    }
    printf("# Received payload matches sent payload\n");

    free(buf_send);
    free(buf_recv);

    return ret;
}

