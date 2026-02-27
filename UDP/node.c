#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "node_defs.h"
#include "node_public.h"

int node_revc(node_t *node) {
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    int rc = recvfrom(node->sock, node->rcv_buffer, BUFFLEN - 1, 0, (struct sockaddr*)&from, &fromlen);
    if (rc == SOCKET_ERROR) {
        printf("recvfrom failed: %d\n", WSAGetLastError());
        return 1;
    }
    node->rcv_buffer[rc] = '\0';
    return 0;
}

int node_send(node_t *node) {
    int msg_len = (int)strlen(node->send_buffer);
    struct sockaddr_in dest;
    int rc; 

    dest.sin_family = AF_INET;
    dest.sin_port   = htons(PORT);
    dest.sin_addr.s_addr = inet_addr("10.97.191.255");
    
    rc = sendto(node->sock, node->send_buffer, msg_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (rc == SOCKET_ERROR) {
        printf("sendto failed: %d\n", WSAGetLastError());
    } else {
        printf("Broadcast sent (%d bytes) to %s:%u\n", rc, inet_ntoa(dest.sin_addr), PORT);
    }
    return 0;
}

void node_cleanup(node_t *node) {
    closesocket(node->sock);
    WSACleanup();
}

int node_init(node_t *node) {
    WSADATA wsa;
    struct sockaddr_in local;
    node->sock = INVALID_SOCKET;
    int enable = 1, rc;

    // Initialize Winsock
    rc = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (rc != 0) {
        printf("WSAStartup failed: %d\n", rc);
        return 1;
    }

    // Create UDP socket
    node->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (node->sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        node_cleanup(node);
        return 1;
    }

    // Broadcast option
    rc = setsockopt(node->sock, SOL_SOCKET, SO_BROADCAST, (const char*)&enable, sizeof(enable));
    if (rc == SOCKET_ERROR) {
        printf("setsockopt(SO_BROADCAST) failed: %d\n", WSAGetLastError());
        node_cleanup(node);
        return 1;
    }

    if(node->type == RECEIVER) {
        // bind
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(PORT);
        rc = bind(node->sock, (struct sockaddr*)&local, sizeof(local));
        if (rc == SOCKET_ERROR) {
            printf("bind failed: %d\n", WSAGetLastError());
            node_cleanup(node);
            return 1;
        }
    }
    return 0;
}