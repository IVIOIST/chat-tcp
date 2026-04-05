#include "common.h"

int init_socket();
int connect_socket(int sockD, uint16_t port);
void print_logo();
void print_menu();
void read_input(char *buffer, size_t size);

struct Client thisClient;

int main(int argc, char* argv[]) {
    print_logo();
    print_menu();
    const int sockD = init_socket();
    const int connectStatus = connect_socket(sockD, 9001);
    if (sockD < 0) {
        perror("socket");
        return 1;
    }

    if (connectStatus == -1) {
        perror("connect");
        return 1;
    }

    while (true) {
        char input[MAX_MSG_LEN + 1];
        read_input(input, sizeof(input));

        if (strcmp(input, "/quit") == 0) {
            send_packet(sockD, MSG_QUIT, NULL, 0);
            printf("Exiting...\n");
            break;
        } else if (strncmp(input, "/name ", 6) == 0) { // NOLINT
            char *new_name = input + 6;

            if (*new_name == '\0') {
                printf("[error] usage: /name <username>\n");
                continue;
            }

            strncpy(thisClient.username, new_name, sizeof(thisClient.username) - 1);
            thisClient.username[sizeof(thisClient.username) - 1] = '\0';

            if (send_packet(sockD, MSG_SET_NAME, thisClient.username, strlen(thisClient.username)) < 0) {
                perror("send_packet");
                break;
            }
        } else if (strcmp(input, "/users") == 0) {
            if (send_packet(sockD, MSG_LIST_CLIENTS, NULL, 0) < 0) {
                perror("send_packet");
                break;
            }
        } else {
            if (send_packet(sockD, MSG_CHAT, input, strlen(input)) < 0) {
                perror("send_packet");
                break;
            }
        }
    }

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

void print_logo() {
    printf("_________ .__            __          ______________________________ \n");
    printf("\\_   ___ \\|  |__ _____ _/  |_        \\__    ___/\\_   ___ \\______   \\\n");
    printf("/    \\  \\/|  |  \\\\__  \\\\   __\\  ______ |    |   /    \\  \\/|     ___/\n");
    printf("\\     \\___|   Y  \\/ __ \\|  |   /_____/ |    |   \\     \\___|    |    \n");
    printf(" \\______  /___|  (____  /__|           |____|    \\______  /____|    \n");
    printf("        \\/     \\/     \\/                                \\/          \n");
}

void print_menu() {
    printf("\n\n");
    printf("             =================Commands=================\n");
    printf("             plain text              -> broadcast\n");
    printf("             /dm <user-id> <message> -> private message\n");
    printf("             /users                  -> list users\n");
    printf("             /name                   -> set username\n");
    printf("             /quit                   -> exit\n\n");
}

void read_input(char* buffer, size_t size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}