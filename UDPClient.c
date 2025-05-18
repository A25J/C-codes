// client_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 9876
#define BUFFER_SIZE 1024

long calculate_latency_microseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_nsec - start.tv_nsec) / 1000L;
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t addr_len = sizeof(serv_addr);
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    ssize_t sent_bytes, recv_bytes;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("> ");
        if (!fgets(send_buffer, BUFFER_SIZE, stdin)) break;
        send_buffer[strcspn(send_buffer, "\n")] = 0;  // Remove newline

        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        sent_bytes = sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if (sent_bytes < 0) {
            perror("sendto failed");
            break;
        }

        if (strcmp(send_buffer, "end") == 0) {
            printf("Exiting.\n");
            break;
        }

        recv_bytes = recvfrom(sockfd, recv_buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
        if (recv_bytes < 0) {
            perror("recvfrom failed");
            break;
        }
        recv_buffer[recv_bytes] = '\0';

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        long latency_us = calculate_latency_microseconds(start_time, end_time);
        double latency_sec = latency_us / 1e6;
        ssize_t total_bytes = sent_bytes + recv_bytes;
        double speed_bytes_per_sec = total_bytes / latency_sec;

        printf("Received msg: %s (length: %zd)\n", recv_buffer, recv_bytes);
        printf("Latency (RTT): %ld µs (%.6f seconds)\n", latency_us, latency_sec);
        printf("Transmission speed: %.2f bytes/sec (%.2f KB/s)\n\n",
               speed_bytes_per_sec, speed_bytes_per_sec / 1024);
    }

    close(sockfd);
    return 0;
}
