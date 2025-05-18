// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 20000
#define BUFFER_SIZE 1024

long calculate_latency_microseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_nsec - start.tv_nsec) / 1000L;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char send_buffer[BUFFER_SIZE] = {0};
    char recv_buffer[BUFFER_SIZE] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    while (1) {
        printf("> ");
        fgets(send_buffer, BUFFER_SIZE, stdin);
        send_buffer[strcspn(send_buffer, "\n")] = 0; // remove newline

        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        // send message to server
        ssize_t bytes_sent = send(sock, send_buffer, strlen(send_buffer), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            break;
        }

        if (strcmp(send_buffer, "end") == 0)
            break;

        memset(recv_buffer, 0, BUFFER_SIZE);

        // read response from server
        ssize_t bytes_received = read(sock, recv_buffer, BUFFER_SIZE - 1);
        if (bytes_received <= 0) {
            perror("Read failed or connection closed");
            break;
        }
        recv_buffer[bytes_received] = 0; // null terminate string

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        long latency_us = calculate_latency_microseconds(start_time, end_time);

        // Calculate speed in bytes per second (bytes_sent + bytes_received) / seconds
        double latency_sec = latency_us / 1e6;
        ssize_t total_bytes = bytes_sent + bytes_received;
        double speed_bytes_per_sec = total_bytes / latency_sec;

        printf("R: %s\n", recv_buffer);
        printf("Latency (RTT): %ld µs (%.6f seconds)\n", latency_us, latency_sec);
        printf("Transmission speed: %.2f bytes/sec (%.2f KB/s)\n\n",
               speed_bytes_per_sec, speed_bytes_per_sec / 1024);
    }

    close(sock);
    return 0;
}
