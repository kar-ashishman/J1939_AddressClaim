#ifndef NODE_PUBLIC_H
#define NODE_PUBLIC_H
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include<pthread.h>
    #include "service.h"
    /*
    typedef struct node_t{
        int(*init_hdlr)(struct node_t *node);
        int(*send_hdlr)(struct node_t *node);
        int(*recv_hdlr)(struct node_t *node);
        void(*cleanup_hdlr)(struct node_t *node);
        SOCKET sock;
        char *rcv_buffer;
        char *send_buffer;
        char type;
        address_table_entry_t address_table_entry;
    } node_t;
    */

    typedef struct node_t{
        //handlers
        int(*init_hdlr)(struct node_t *node);
        int(*send_hdlr)(struct node_t *node);
        int(*recv_hdlr)(struct node_t *node);
        void(*cleanup_hdlr)(struct node_t *node);
        //socket tied to the node
        SOCKET sock;
        //receive and send buffers
        char *rcv_buffer;
        char *send_buffer;
        // Every node has a 64 bit NAME
        char name;
        // Source address
        int sa;
        //mutex lock for the node
        pthread_mutex_t mutex_lock;
        //condition waiting
        pthread_cond_t cond_wait;
        //status of node
        int8_t stat;
    } node_t;

#endif // NODE_PUBLIC_H
