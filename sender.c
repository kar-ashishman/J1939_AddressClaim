#include "node_public.h"
#include "node_defs.h"
#include "node_proto.h"

static char buffer[] = "Hello from sender";
node_t node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = NULL,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .send_buffer = buffer,
    .rcv_buffer = NULL,
    .type = SENDER
};

int main() {
    
    // Initialize the node (socket setup)
    if (node.init_hdlr(&node) != 0) {
        return 1;
    }

    // Send a broadcast message
    node.send_hdlr(&node);

    // Cleanup the node (socket cleanup)
    node.cleanup_hdlr(&node);
    return 0;
}