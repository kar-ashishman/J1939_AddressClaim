#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_proto.h"
#include "node_defs.h"

unsigned char send_buffer[BUFFER_LENGTH] = {0x18,0xEE,SOURCE_ADDRESS,0xFF,0,0,0,0,0,0,0,0,0};
unsigned char recv_buffer[BUFFER_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

node_t node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_recv,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET_TYPE,
    .send_buffer = send_buffer,
    .rcv_buffer = recv_buffer,
    .sa = SOURCE_ADDRESS,
    .name = {0,2,1,1,1,0,6,3},
    .state = NODE_STATE_UNCLAIMED,
};


int main() {
    printf("Starting Address Claim Simulation...\n");
    // Initialize node
    node.init_hdlr(&node);

    // Start a thread for sending a message
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, (void *)address_claim, &node);

    // Start a thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, (void *)address_claim_parser, &node);

    // Wait for threads to finish
    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
    node.cleanup_hdlr(&node);

    return 0;
}