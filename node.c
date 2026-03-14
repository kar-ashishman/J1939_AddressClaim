#include <stdio.h>
#include "node_defs.h"
#include "node_public.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define GET_ERROR() WSAGetLastError()
    #define CLOSE_SOCKET closesocket
    #define SOCKET_TYPE SOCKET
    #define INVALID_SOCKET_TYPE INVALID_SOCKET
    #define SOCKET_ERROR_TYPE SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #define GET_ERROR() errno
    #define CLOSE_SOCKET close
    #define SOCKET_TYPE int
    #define INVALID_SOCKET_TYPE -1
    #define SOCKET_ERROR_TYPE -1
#endif
int node_revc(node_t *node) {
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    int rc = recvfrom(node->sock, node->rcv_buffer, BUFFLEN - 1, 0, (struct sockaddr*)&from, &fromlen);
    if (rc == SOCKET_ERROR_TYPE) {
        printf("recvfrom failed: %d\n", GET_ERROR());
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
    dest.sin_addr.s_addr = inet_addr("10.97.177.225");
    
    rc = sendto(node->sock, node->send_buffer, msg_len, 0, (struct sockaddr*)&dest, sizeof(dest));
    if (rc == SOCKET_ERROR_TYPE) {
        printf("sendto failed: %d\n", GET_ERROR());
    } else {
        printf("Broadcast sent (%d bytes) to %s:%u\n", rc, inet_ntoa(dest.sin_addr), PORT);
    }
    return 0;
}

void node_cleanup(node_t *node) {
    CLOSE_SOCKET(node->sock);
#ifdef _WIN32
    WSACleanup();
#endif
}

int node_init(node_t *node) {
    struct sockaddr_in local;
    node->sock = INVALID_SOCKET_TYPE;
    int enable = 1, rc;

#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsa;
    rc = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (rc != 0) {
        printf("WSAStartup failed: %d\n", rc);
        return 1;
    }
#endif

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
            printf("bind failed: %d\n", WSAGetLastError());
            node_cleanup(node);
            return 1;
        }
    }
    return 0;
}