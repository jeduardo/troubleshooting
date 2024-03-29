#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

int sockfd;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void sigint_handler(int sig) {
    printf("Caught signal %d, closing socket and exiting...\n", sig);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    int sockfd, newsockfd, portno;
    socklen_t len;
    struct sockaddr_in6 serv_addr, cli_addr;
    int n, base_size, opt, buf_size, count, mss = 0;
    char *buffer;

    if (argc < 3) {
        fprintf(stderr, "Usage: port base_size [mss]\n");
        exit(EXIT_FAILURE);
    }

    if (argc > 3) mss = atoi(argv[3]);

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        error("Error setting up signal handler");

    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        error("ERROR on setting SO_REUSEADDR");

    opt = 0;
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0)
        error("ERROR on setting IPV6_V6ONLY");

    if (mss > 0)
        if (setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss)) < 0)
            error("setsockopt TCP_MAXSEG failed");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    base_size = atoi(argv[2]);
    buf_size = 3 * base_size;

    printf("# Base size: %d\n", base_size);
    printf("# Payload size: %d\n", buf_size);

    buffer = calloc(buf_size, sizeof(char));

    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    len = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &len);
        if (newsockfd < 0) error("ERROR on accept");
        printf("# New connection\n");

        len = sizeof(mss);
        if (getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &len) < 0)
            error("getsockopt TCP_MAXSEG failed");
        printf("# MSS is %d\n", mss);

        count = 0;
        bzero(buffer, buf_size);

        while ((buf_size - count) > 0) {
            n = read(newsockfd, buffer + count, buf_size - count);
            if (n < 0) error("ERROR reading from socket");
            printf("<- %d bytes read\n", n);
            if (n == 0) break;  // EOF
            count += n;
        }
        printf("# Total bytes read: %d\n", count);

        n = write(newsockfd, buffer, count);
        if (n < 0) error("ERROR writing to socket");
        printf("-> %d bytes written\n", n);

        close(newsockfd);
        printf("# Connection closed\n");
    }

    close(sockfd);
    free(buffer);
    return 0;
}

