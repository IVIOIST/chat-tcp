#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define MAX_NAME_LEN 32
#define MAX_MSG_LEN 1024

enum MessageType {
    MSG_SET_NAME = 1,
    MSG_CHAT = 2,
    MSG_JOIN = 3,
    MSG_LEAVE = 4,
    MSG_NOTICE = 5,
};

struct MessageHeader {
    enum MessageType type;
    uint32_t payload_len;
};

int send_all(int sockfd, const void* buffer, size_t length);
int recv_all(int sockfd, void* buffer, size_t length);

int send_packet(int sockfd, enum MessageType type, const void* payload, uint32_t payload_len);
int recv_packet(int sockfd, enum MessageType* type, void* payload_buffer, uint32_t buffer_size, uint32_t* payload_len);

