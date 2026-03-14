#ifndef NODE_PUBLIC_H
#define NODE_PUBLIC_H
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include "service.h"

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

#endif // NODE_PUBLIC_H