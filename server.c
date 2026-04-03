#include "common.h"

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

        enum MessageType type;
        uint32_t payload_len;
        char buffer[1024];
        while (recv_packet(clientSocket, &type, buffer, 1024, &payload_len) == 0) {
            buffer[payload_len] = '\0';
            printf("Received (%u bytes): %s\n", payload_len, buffer);
            fflush(stdout);
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
