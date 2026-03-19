#ifndef NODE_PROTO_H
#define NODE_PROTO_H

    #include "node_public.h"

    void node_recv(node_t *node);
    void node_send(node_t *node);
    int node_init(node_t *node);
    void node_cleanup(node_t *node);
    void recv_address_claim_frames(void *arg);


    // Other protos
    void address_claim(node_t* node);
    void address_claim_parser(node_t* node);

#endif // NODE_PROTO_H