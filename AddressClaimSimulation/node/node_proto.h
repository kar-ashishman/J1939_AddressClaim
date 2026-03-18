#ifndef NODE_PROTO_H
#define NODE_PROTO_H

    #include "node_public.h"

    void node_recv(node_t *node);
    void node_send(node_t *node);
    int node_init(node_t *node);
    void node_cleanup(node_t *node);
    void recv_address_claim_frames(void *arg);

#endif // NODE_PROTO_H