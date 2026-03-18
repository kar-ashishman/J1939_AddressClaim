#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_defs.h"
#include <sys/time.h>


void address_claim(node_t* node) {
    pthread_t th;
    pthread_attr_t th_attr;
    pthread_attr_setdetachstate(&th_attr, NULL);

    // Attempts a max of MAX_ATTEMPTS to claim an address
    node->send_buffer[0] = 0x18; 
    node->send_buffer[1] = 0xEE;
    node->send_buffer[2] = node->sa; // Source address
    node->send_buffer[3] = 0xFF; // Destination address (broadcast)

    // Send the claim message
    pthread_create(&th, &th_attr, (void *)node->send_hdlr, node);
    if (node->state == NODE_STATE_UNCLAIMED) {
        node->node_start_time = current_time_millis();
        node->state = NODE_STATE_CLAIMING;
    }
} /* End of address_claim */

void address_claim_parser(node_t* node) {
    long long current_time = current_time_millis();
    static char attempts = 0;

    // Dormant node doesnt respond to address claims
    if (node->rcv_buffer[1] != 0xEE || node->state == NODE_STATE_CLAIM_FAILED) return;
   
    // Check if self address claim is completed
    if(attempts > MAX_ATTEMPTS) {
        node->state = NODE_STATE_CLAIM_FAILED;
        return;
    }

    if(node->state == NODE_STATE_CLAIMED && node->rcv_buffer[2] == node->sa && node->rcv_buffer[3] == 0xFF) {
        printf("Address claim conflict detected. Checking NAME conflict..\n");
    }

    if(node->state == NODE_STATE_CLAIMING || node->state == NODE_STATE_UNCLAIMED) {
        // check if wait time is elapsed for the claim attempt
        if(current_time - node->node_start_time >= TIMEOUT_MSECS) {
            node->node_start_time = current_time;
            attempts++;
            node->sa++;
            address_claim(node);
        }
    }



    

    // if it is address claim

    // check if the source address matches our own
    if (node->rcv_buffer[2] == node->sa) {}
} /* End of address_claim_parser */