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
    int sockfd, portno, n, payload_size;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *buffer;

    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port payload_size\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    payload_size = 3 * atoi(argv[3]);

    buffer = calloc(payload_size, sizeof(char));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Prepare payload
    memset(buffer, 'a', payload_size);
    memset(buffer + payload_size, 'b', payload_size);
    memset(buffer + 2 * payload_size, 'c', payload_size);

    // Send payload
    n = write(sockfd, buffer, 3 * payload_size);
    if (n < 0) error("ERROR writing to socket");
    printf("-> %d bytes sent\n", n);
    ;
    // Receive response
    bzero(buffer, payload_size);
    n = read(sockfd, buffer, payload_size);
    if (n < 0) error("ERROR reading from socket");
    printf("Received: %s\n", buffer);
    printf("<- %d bytes received\n", n);

    close(sockfd);

    return 0;
}

