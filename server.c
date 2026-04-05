#include "common.h"


struct Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int init_socket();
int bind_socket(int sockD, uint16_t port);
void* handle_client(void* arg);

int send_to_all_clients(enum MessageType type, const char* message, uint32_t message_len);
int send_to_client(int sockfd, enum MessageType type, const char* message, uint32_t message_len);

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

        int client_idx = -1;
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                client_idx = i;
                clients[i].sockfd = clientSocket;
                clients[i].active = true;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (client_idx == -1) {
            fprintf(stderr, "Max clients reached. Rejecting connection.\n");
            close(clientSocket);
            continue;
        }

        int* client_arg = malloc(sizeof(int));
        if (client_arg == NULL) {
            perror("malloc");
            close(clientSocket);
            continue;
        }

        *client_arg = client_idx;
        pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, handle_client, client_arg) != 0) {
            perror("pthread_create");

            pthread_mutex_lock(&clients_mutex);
            clients[client_idx].active = false;
            pthread_mutex_unlock(&clients_mutex);

            close(clientSocket);
            free(client_arg);
            continue;
        }

        pthread_detach(thread_id); // Detach thread to clean up resources when it finishes

    }
    close(servSockD);
    return 0;
}

void* handle_client(void* arg) {
    int clientIdx = *(int *)arg;
    free(arg);

    enum MessageType type;
    char buffer[MAX_MSG_LEN+1];
    uint32_t payload_len;

    while (recv_packet(clients[clientIdx].sockfd, &type, buffer, 1024, &payload_len) == 0) {
        switch (type) {
            case MSG_CHAT:
                printf("Received (%u bytes): %s\n", payload_len, buffer);
                send_to_all_clients(MSG_CHAT, buffer, strlen((char*)buffer));
                fflush(stdout);
                break;
            case MSG_SET_NAME:
                printf("Received (%u bytes) Set Username: buffer: %s\n", payload_len, buffer);
                strncpy(clients[clientIdx].username, buffer, MAX_NAME_LEN);
                break;
            case MSG_LIST_CLIENTS:
                printf("Active Clients:\n");
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i].active) {
                        printf("Client Name: %s     |", clients[i].username);
                        printf("     Client ID: %d\n", clients[i].sockfd);
                    }
                }
                break;
            case MSG_QUIT:
                printf("Client quitting: %s\n", clients[clientIdx].username);
                clients[clientIdx].active = false;
                close(clients[clientIdx].sockfd);
                return NULL;
            default:
                fprintf(stderr, "Unknown message type: %d\n", type);
                break;
        }
    }
    return NULL;
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

int send_to_all_clients(enum MessageType type, const char* message, uint32_t message_len) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            if (send_packet(clients[i].sockfd, type, message, message_len) < 0) {
                perror("send_packet");
                clients[i].active = false;
                close(clients[i].sockfd);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    return 0;
}

int send_to_client(int sockfd, enum MessageType type, const char* message, uint32_t message_len) {
    if (send_packet(sockfd, type, message, message_len) < 0) {
        perror("send_packet");
        return -1;
    }
    return 0;
}