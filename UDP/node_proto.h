#ifndef NODE_PROTO_H
#define NODE_PROTO_H

    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include "node_public.h"

    int node_revc(node_t *node);
    int node_send(node_t *node);
    int node_init(node_t *node);
    void node_cleanup(node_t *node);

#endif // NODE_PROTO_H
