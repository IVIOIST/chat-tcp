#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, int const* argv[]) {
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    char serMsg[255] = "This is a message from the server to the client: hello client";

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr));

    listen(servSockD, 1);

    while (true) {
        int clientSocket = accept(servSockD, NULL, NULL);

        char strMsg[255];
        int bytesRead;

        while ((bytesRead = recv(clientSocket, strMsg, sizeof(strMsg) - 1, 0)) > 0) {
            strMsg[bytesRead] = '\0';
            printf("Message: %s", strMsg);
        }
    }
    close(servSockD);
    return 0;
}
