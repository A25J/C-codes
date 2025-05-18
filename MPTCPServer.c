#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Standard headers for input/output (stdio.h),
// memory management (stdlib.h, string.h), 
// string operations (ctype.h), 
// system calls (unistd.h), 
// and socket programming (netinet/in.h, sys/socket.h).

#ifndef IPPROTO_MPTCP
#define IPPROTO_MPTCP 262
#endif

//Defines IPPROTO_MPTCP if not already defined (some systems may not have it in their headers).
//  262 is the protocol number assigned to MPTCP by IANA.

#define PORT 20000
#define BUFFER_SIZE 1024

//Constants: the port the server listens on and the size of the message buffer.

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper(*str);
        str++;
    }
}
//A utility function to convert a string to uppercase, character by character.

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    //Define server socket variables:

    //server_fd: listening socket.

    //new_socket: accepted client connection.

    //address: structure that holds the server address.

    //buffer: message buffer for communication.

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_MPTCP); // Use MPTCP

    //Create a socket with:

    //AF_INET: IPv4

    //SOCK_STREAM: stream socket (like TCP)

    //IPPROTO_MPTCP: Multipath TCP instead of standard TCP.

    if (server_fd == -1) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    //Error handling if socket creation fails.

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //Set up the server address:

    //AF_INET: IPv4.

    //INADDR_ANY: bind to all available interfaces.

    //htons(PORT): convert port to network byte order

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    //Bind the socket to the specified address and port.

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    //Listen for incoming connections, with a backlog of 3.

    printf("Waiting for a connection...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("New connection accepted.\n");
    //Accept an incoming connection. Once connected, the server prints confirmation.

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) break;
        //Start a loop to handle client messages:
        //Clear the buffer.
        //Read from the client.
        //Exit loop if no data is received.
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "end") == 0) {
            printf("Bye\n");
            break;
        }
        //Remove newline character.
        //If message is "end", break the loop (terminate session).

        printf("Message: %s\n", buffer);
        to_uppercase(buffer);
        send(new_socket, buffer, strlen(buffer), 0);
        //Print received message.
        //Convert it to uppercase.
        //Send back the uppercase message to the client.
    }

    close(new_socket);
    close(server_fd);
    return 0;
    //Close sockets and exit.
}
