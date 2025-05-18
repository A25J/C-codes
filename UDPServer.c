// server_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 9876
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *) &cli_addr, &cli_len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0'; // Null terminate received data

        printf("Received message: %s (length %zd) from %s:%d\n", 
               buffer, n,
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // Echo the message back (you can modify it if needed)
        // For example, uppercase it (simple)
        for (int i = 0; i < n; i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z')
                buffer[i] -= 32;
        }

        ssize_t sent = sendto(sockfd, buffer, n, 0, (struct sockaddr *) &cli_addr, cli_len);
        if (sent < 0) {
            perror("sendto failed");
        }
    }

    close(sockfd);
    return 0;
}
