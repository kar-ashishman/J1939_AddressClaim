#include "node_public.h"
#include "node_defs.h"
#include "node_proto.h"
#include <stdio.h>

static char buffer[BUFFLEN];
node_t node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_revc,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .rcv_buffer = buffer,
    .send_buffer = NULL,
    .type = RECEIVER
};

int main() {
   
    // Initialize the node (socket setup)
    if (node.init_hdlr(&node) != 0) {
        return 1;
    }

    // Receive a broadcast message
    node.recv_hdlr(&node);
    printf("Received message: %s\n", node.rcv_buffer);

    // Cleanup the node (socket cleanup)
    node.cleanup_hdlr(&node);
    return 0;
}