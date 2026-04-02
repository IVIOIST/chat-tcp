#include "common.h"


int main(int argc, char* argv[]) {
    int sockD = socket(AF_INET, SOCK_STREAM, 0);

    if (sockD < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus = connect(sockD, (struct sockaddr*) &servAddr, sizeof(servAddr));

    if (connectStatus == -1) {
        perror("connect");
        return 1;
    }

    char strData[255];
    while (fgets(strData, sizeof(strData), stdin) != NULL) {
        send(sockD, strData, strlen(strData), 0);
    }

    close(sockD);
    return 0;
}
