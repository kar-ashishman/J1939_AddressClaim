#include <stdio.h>
#include <unistd.h>
#include "../node/node_public.h"
#include "../node/node_proto.h"
#include "../node/node_defs.h"

unsigned char send_buffer[BUFFER_LENGTH];
unsigned char recv_buffer[BUFFER_LENGTH];

node_t recv_node = {
    .init_hdlr = node_init,
    .send_hdlr = NULL,
    .recv_hdlr = node_recv,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET_TYPE,
    .rcv_buffer = send_buffer,
    .send_buffer = NULL,
    .type = RECEIVER
};

node_t send_node = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = NULL,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET_TYPE,
    .rcv_buffer = recv_buffer,
    .send_buffer = "Something",
    .type = SENDER
};

int main() {
    printf("Starting Address Claim Simulation...\n");
    // Initialize nodes
    recv_node.init_hdlr(&recv_node);
    send_node.init_hdlr(&send_node);

    // Start a thread for receiving messages
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, (void *)recv_node.recv_hdlr, &recv_node);
    
    sleep(1);
    printf("Trying to send %s\n", send_node.send_buffer);
    
    // Start a thread for sending a message
    pthread_t send_thread;
    pthread_attr_t send_attr;
    pthread_attr_setdetachstate(&send_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&send_thread, &send_attr, (void *)send_node.send_hdlr, 
                    &send_node);

    // Wait for the receiving thread to finish
    pthread_join(recv_thread, NULL);
    recv_node.cleanup_hdlr(&recv_node);

    return 0;
}