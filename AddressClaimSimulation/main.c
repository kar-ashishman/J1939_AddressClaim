#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_proto.h"
#include "node_defs.h"

unsigned char send_buffer[BUFFER_LENGTH];
unsigned char recv_buffer[BUFFER_LENGTH];

node_t node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_recv,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET_TYPE,
    .send_buffer = send_buffer,
    .rcv_buffer = recv_buffer,
    .sa = SOURCE_ADDRESS,
    .state = NODE_STATE_UNCLAIMED,
    .node_start_time = 0
};



int main() {
    printf("Starting Address Claim Simulation...\n");
    // Initialize node
    node.init_hdlr(&node);

    // Start a thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, (void *)node.recv_hdlr, &node);
    
    // Start a thread for sending a message
    address_claim(&node);

    // Wait for the receiving thread to finish
    pthread_join(recv_thread, NULL);
    node.cleanup_hdlr(&node);

    return 0;
}