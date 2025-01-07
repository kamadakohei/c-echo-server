#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    const char *host = DEFAULT_HOST;
    int port = DEFAULT_PORT;

    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3) {
        port = atoi(argv[2]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Invalid port number.\n");
            return EXIT_FAILURE;
        }
    }

    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        error("Socket creation failed");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        error("Invalid host address");
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Connection to server failed");
    }

    printf("Connected to %s:%d\n", host, port);

    while (1) {
        printf("Enter message (or 'exit' to quit): ");
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
            fprintf(stderr, "Failed to read input.\n");
            break;
        }

        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            printf("Exiting.\n");
            break;
        }

        strcat(message, "\n");
        // メッセージ送信
        if (send(sock, message, strlen(message), 0) < 0) {
            error("Send failed");
        }

        ssize_t n = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (n < 0) {
            error("Receive failed");
        }

        buffer[n] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    close(sock);
    printf("Connection closed.\n");

    return 0;
}
