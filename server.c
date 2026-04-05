#include "common.h"


struct Client clients[MAX_CLIENTS];

int init_socket();
int bind_socket(int sockD, uint16_t port);


int main(int argc, char const* argv[]) {
    int servSockD = init_socket();
    int bindStatus = bind_socket(servSockD, 9001);

    if (bindStatus < 0) {
        fprintf(stderr, "Failed to bind socket\n");
        return EXIT_FAILURE;
    }

    listen(servSockD, 1);

    while (true) {
        int clientSocket = accept(servSockD, NULL, NULL);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }

        enum MessageType type;
        char buffer[MAX_MSG_LEN+1];
        uint32_t payload_len;

        while (recv_packet(clientSocket, &type, buffer, 1024, &payload_len) == 0) {
            switch (type) {
                case MSG_CHAT:
                    buffer[payload_len] = '\0'; // Null-terminate the received message
                    printf("Received (%u bytes): %s\n", payload_len, buffer);
                    fflush(stdout);
                    break;
                case MSG_SET_NAME:
                    printf("Client set name: %.*s\n", payload_len, buffer);
                    break;
                default:
                    fprintf(stderr, "Unknown message type: %d\n", type);
                    break;
            }
        }
    }
    close(servSockD);
    return 0;
}



int init_socket() {
    const int sockD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockD < 0) {
        perror("socket");
        return -1;
    }
    return sockD;
}

int bind_socket(int sockD, uint16_t port) {
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        close(sockD);
        return -1;
    }
    return 0;
}
