#include "common.h"

int send_all(int sockfd, const void *buffer, size_t length) {
    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent = send(sockfd, buffer + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            return -1; // Error or connection closed
        }
        total_sent += sent;
    }
    return 0;
}

int recv_all(int sockfd, void *buffer, size_t length) {
    size_t total_received = 0;

    while (total_received < length) {
        ssize_t received = recv(sockfd, buffer + total_received, length - total_received, 0);
        if (received <= 0) {
            return -1; // Error or connection closed
        }
        total_received += received;
    }
    return 0;
}

int send_packet(int sockfd, enum MessageType type, const void *payload, uint32_t payload_len) {
    // build header
    const struct MessageHeader header = {
        .type = htonl(type),
        .payload_len = htonl(payload_len)
    };

    if (send_all(sockfd, &header, sizeof(header)) < 0) {
        return -1; // Error sending header
    }

    if (payload_len > 0 && payload != NULL) {
        if (send_all(sockfd, payload, payload_len) < 0) {
            return -1; // Error sending payload
        }
    }
    return 0;
}

int recv_packet(int sockfd, enum MessageType *type, void *payload_buffer, uint32_t buffer_size, uint32_t *payload_len) {
    struct MessageHeader temp = {0};

    if (type == NULL || payload_len == NULL) {
        return -1; // Invalid arguments
    }

    if (recv_all(sockfd, &temp, sizeof(temp)) < 0) {
        return -1;
    }

    enum MessageType host_type = ntohl(temp.type);
    uint32_t host_payload_len = ntohl(temp.payload_len);

    if (host_payload_len > buffer_size) {
        return -1; // Payload too large for buffer
    }

    if (host_payload_len > 0 && payload_buffer == NULL) {
        return -1; // Buffer required for payload
    }

    if (host_payload_len > 0) {
        if (recv_all(sockfd, payload_buffer, host_payload_len) < 0) {
            return -1; // Error receiving payload
        }
    }

    *type = host_type;
    *payload_len = host_payload_len;
    return 0;
}
