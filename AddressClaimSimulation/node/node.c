#include <stdio.h>
#include "node_defs.h"
#include "node_public.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

void node_cleanup(node_t *node) {
    close(node->sock);
} /* End of node_cleanup */

int node_init(node_t *node) {
    printf("Initializing node...\n");
    struct sockaddr_in local;
    node->sock = INVALID_SOCKET_TYPE;
    int enable = 1, rc;

    // Create UDP socket
    node->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (node->sock ==   INVALID_SOCKET_TYPE) {
        printf("socket failed: %d\n", GET_ERROR());
        node_cleanup(node);
        return 1;
    }

    // Broadcast option
    rc = setsockopt(node->sock, SOL_SOCKET, SO_BROADCAST, (const char*)&enable, sizeof(enable));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("setsockopt(SO_BROADCAST) failed: %d\n", GET_ERROR());
        node_cleanup(node);
        return 1;
    }

    if(node->type == RECEIVER) {
        // bind
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(PORT);
        rc = bind(node->sock, (struct sockaddr*)&local, sizeof(local));
        if (rc == SOCKET_ERROR_TYPE) {
            printf("bind failed\n");
            node_cleanup(node);
            return 1;
        }
    }
    return 0;
} /* End of node_init */

void node_send(node_t *node) {
    printf("Sending message...\n");
    int msg_len = (int)strlen(node->send_buffer);
    struct sockaddr_in dest;
    int rc; 

    dest.sin_family = AF_INET;
    dest.sin_port   = htons(PORT);
    dest.sin_addr.s_addr = inet_addr(GLOBALIP);
    
    rc = sendto(node->sock, node->send_buffer, msg_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("sendto failed: %d\n", GET_ERROR());
    } else {
        printf("Broadcast sent (%d bytes) to %s:%u\n", rc, inet_ntoa(dest.sin_addr), PORT);
    }
} /* End of node_send */


void node_recv(node_t *node) {
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    printf("Waiting for messages...\n");
    int rc = recvfrom(node->sock, node->rcv_buffer, 
        BUFFLEN - 1, 0, (struct sockaddr*)&from, &fromlen);
    if (rc == SOCKET_ERROR_TYPE) {
        printf("recvfrom failed: %d\n", GET_ERROR());
    }
    node->rcv_buffer[rc] = '\0';
    printf("Got%s\n", node->rcv_buffer);
} /* End of node_recv */