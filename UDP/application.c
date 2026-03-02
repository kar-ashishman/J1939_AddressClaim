#include "node_public.h"
#include "node_defs.h"
#include "node_proto.h"
#include <windows.h>

static unsigned char send_buffer[12];
static unsigned char rcv_buffer[12];

node_t sending_node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = NULL,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .send_buffer = send_buffer,
    .rcv_buffer = NULL,
    .type = SENDER
};

node_t receiving_node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_recv,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .send_buffer = NULL,
    .rcv_buffer = rcv_buffer,
    .type = RECEIVER
};

addr_claim_t addr_claim = {
    .sa = 0x01,
    .name = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22}
};

addr_claim_t addr_table[100];


int main() {
    // Initialize sending and receiving nodes
    if (sending_node.init_hdlr(&sending_node) != 0) {
        printf("Failed to initialize sending node\n");
        return 1;
    }

    if (receiving_node.init_hdlr(&receiving_node) != 0) {
        printf("Failed to initialize receiving node\n");
        sending_node.cleanup_hdlr(&sending_node);
        return 1;
    }

    // Create a receiving thread
    HANDLE h = CreateThread(NULL, 0, recv_address_claim_frames, &receiving_node, 0, NULL);
    WaitForSingleObject(h, 0);

    // Send address claim frame from sending node
    unsigned char address_claim_frame[12] = {0x00, 0xEE, 0xFF, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};
}