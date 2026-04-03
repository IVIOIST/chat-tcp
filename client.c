#include "common.h"

int init_socket();
int connect_socket(int sockD, uint16_t port);

int main(int argc, char* argv[]) {
    int sockD = init_socket();
    int connectStatus = connect_socket(sockD, 9001);
    if (sockD < 0) {
        perror("socket");
        return 1;
    }

    if (connectStatus == -1) {
        perror("connect");
        return 1;
    }

    char strData[255];
    while (fgets(strData, sizeof(strData), stdin) != NULL) {
        size_t len = strlen(strData);
        if (len > 0 && strData[len - 1] == '\n') {
            strData[len - 1] = '\0'; // Remove newline character
            len--;
        }

        if (send_packet(sockD, MSG_CHAT, strData, len) == -1) {
            perror("send_packet");
            break;
        }
    }

    close(sockD);
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

int connect_socket(int sockD, uint16_t port) {
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockD);
        return -1;
    }

    if (connect(sockD, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        close(sockD);
        return -1;
    }
    return 0;
}