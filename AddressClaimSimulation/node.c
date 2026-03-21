#include <stdio.h>
#include "node_defs.h"
#include "node_public.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "address_claim_proto.h"

void printmsg(unsigned char* buffer) {
    for(int i = 0; i < BUFFER_LENGTH; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
} /* End of printmsg */

void node_cleanup(node_t *node) {
    close(node->sock);
    pthread_mutex_destroy(&node->lock1);
    pthread_cond_destroy(&node->recv_cond1);
    pthread_mutex_destroy(&node->lock2);
    pthread_cond_destroy(&node->recv_cond2);
} /* End of node_cleanup */

int node_init(node_t *node) {
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

    // Allow multiple processes to bind to the same UDP port (broadcast listener)
    rc = setsockopt(node->sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("setsockopt(SO_REUSEADDR) failed: %d\n", GET_ERROR());
        node_cleanup(node);
        return 1;
    }

    #ifdef SO_REUSEPORT
    // SO_REUSEPORT allows multiple sockets to bind the same port (Linux/BSD)
    rc = setsockopt(node->sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&enable, sizeof(enable));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("setsockopt(SO_REUSEPORT) failed: %d\n", GET_ERROR());
        node_cleanup(node);
        return 1;
    }
    #endif

    // Broadcast option
    rc = setsockopt(node->sock, SOL_SOCKET, SO_BROADCAST, (const char*)&enable, sizeof(enable));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("setsockopt(SO_BROADCAST) failed: %d\n", GET_ERROR());
        node_cleanup(node);
        return 1;
    }

    // bind (accept packets sent to any address on this host)
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(PORT);
    rc = bind(node->sock, (struct sockaddr*)&local, sizeof(local));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("bind failed\n");
        node_cleanup(node);
        return 1;
    }

    // Initialize mutex and condition variable
    pthread_mutex_init(&node->lock1, NULL);
    pthread_mutex_init(&node->lock2, NULL);
    pthread_cond_init(&node->recv_cond1, NULL);
    pthread_cond_init(&node->recv_cond2, NULL);

    // Initialize sendbuffer with name
    node->send_buffer[0] = 0x18;
    node->send_buffer[1] = 0xEE;
    node->send_buffer[2] = node->sa;
    node->send_buffer[3] = 0xFF;

    memcpy(&node->send_buffer[4], node->name, 8);
    printf("Address claim simulation...\n");
    printf("Initialized node.........................\n");

    return 0;
} /* End of node_init */

void node_send(node_t *node) {
    int msg_len = BUFFER_LENGTH;
    struct sockaddr_in dest;
    int rc; 

    dest.sin_family = AF_INET;
    dest.sin_port   = htons(PORT);
    dest.sin_addr.s_addr = inet_addr(GLOBALIP);
    
    rc = sendto(node->sock, node->send_buffer, msg_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("sendto failed: %d\n", GET_ERROR());
        return;
    }
    printf("Sent message: ");
    printmsg(node->send_buffer);
} /* End of node_send */


void node_recv(node_t *node) {
    struct sockaddr_in from;
    int rc;
    socklen_t fromlen = sizeof(from);
    rc = recvfrom(node->sock, node->rcv_buffer, 
        BUFFER_LENGTH, 0, (struct sockaddr*)&from, &fromlen);
    if (rc == SOCKET_ERROR_TYPE) {
        printf("recvfrom failed: %d\n", GET_ERROR());
    }
    printf("Received message: ");
    printmsg(node->rcv_buffer);
} /* End of node_recv */

void node_table_printer(node_t *node) {
    for(int i=0; i<256; i++) {
        if(node->table[i][8]) { // if table entry is updated
            printf("%d -> ", i);
            for(int j=0; j<8; j++)
                printf("%d ", node->table[i][j]);
            printf("\n");
        }
    }
}