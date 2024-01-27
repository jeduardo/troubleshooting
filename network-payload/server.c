#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;  // Changed type to socklen_t
    struct sockaddr_in serv_addr, cli_addr;
    int n, payload_size, opt;
    char *buffer;

    if (argc < 3) {
        fprintf(stderr, "ERROR, no port or payload_size provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        error("ERROR on setsockopt");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    payload_size = 3 * atoi(argv[2]);
    printf("# Payload size chosen: %d\n", payload_size);
    buffer = calloc(payload_size, sizeof(char));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        printf("# New connection\n");

        bzero(buffer, payload_size);
        n = read(newsockfd, buffer, payload_size);
        if (n < 0) error("ERROR reading from socket");
        printf("<- %d bytes read\n", n);

        n = write(newsockfd, buffer, payload_size);
        if (n < 0) error("ERROR writing to socket");
        printf("-> %d bytes written\n", n);

        close(newsockfd);
        printf("# Connection closed\n");
    }

    close(sockfd);
    free(buffer);
    return 0;
}

