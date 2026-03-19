#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_defs.h"
#include <sys/time.h>
#include <errno.h>


#include "address_claim_proto.h"

void address_claim(node_t* node) {
    // updating network address table

    while(1) {
        // wait for a signal from receiving thread to start claiming address
        // This happens in two cases:
        // 1. When the node is initialized and is ready to claim address
        // 2. When the node receives a message with same SA as its own SA,
        //    which means there is an address conflict and it needs to claim a new address
        printf("Try locking\n");
        pthread_mutex_lock(&node->lock);
        printf("Lock accessed\n");
        pthread_cond_wait(&node->recv_cond, &node->lock);
        printf("woke up\n");

        // try to claim address by sending addr claim message
        // Wait for a signal from rcving thread 
        // Timeout of 250ms for receiving a message
        // After timeout, claim the address
        while(node->state != NODE_STATE_CLAIMED) {
            node->send_hdlr(node);
            int rc = pthread_cond_timedwait(&node->recv_cond, &node->lock, 
                &(struct timespec){.tv_sec = 0, .tv_nsec = TIMEOUT_MSECS * 1000000});
            if (rc == ETIMEDOUT) {
                // address claim successful
                // update address table
                // unlock mutex and break
                node->state = NODE_STATE_CLAIMED;
                // code for updating address table

                printf("Address claim successful. SA: %d\n", node->sa);
                break; // No messages received within timeout, proceed to claim
            } else {
                // if all secondary addresses are exhausted, address claim failed
                if(node->sa == SECONDARY_ADDRESS_END) {
                    printf("Address claim failed. No available addresses.\n");
                    return;
                }
                // if last claim was base sa, go to sec sa
                // else claim next of last sa
                node->sa = node->sa == SOURCE_ADDRESS ? 
                        SECONDARY_ADDRESS : node->sa+1;
            }
        }
    }
} /* End of address_claim */


void address_claim_parser(node_t* node) {
    int result;

    // signal sender thread to start claiming address
    pthread_mutex_lock(&node->lock);
    pthread_cond_signal(&node->recv_cond);
    pthread_mutex_unlock(&node->lock);

    while(1) {
        // Wait for incoming messages and parse them
        node->recv_hdlr(node);

        // Check if name of the incoming message is same as the node's name
        if (name_comparator(&(node->rcv_buffer[4]), 
                node->name) == 0)
            continue; // Ignore messages from self

        // if incoming message is from a different SA
        if(node->rcv_buffer[2] != node->sa) {
            // code for updating address table
        } else { 
            // if incoming message is from same SA
            // address conflict
            // verify name superiority
            result = name_comparator(&(node->rcv_buffer[4]), node->name);
            if(result == NAME2_GREATER_THAN_NAME1)
                node->send_hdlr(node);
            else {
                // higher priority node has claimed the address, so this node needs to claim a new address
                // signal sender thread to claim new address
                pthread_mutex_lock(&node->lock);
                node->state = NODE_STATE_CLAIMING;
                pthread_cond_signal(&node->recv_cond);
                pthread_mutex_unlock(&node->lock);
            }
        }
    }
        
} /* End of address_claim_parser */



int name_comparator(unsigned char* name1, unsigned char* name2) {
    for (int i = 0; i < 8; i++) {
        if (name1[i] > name2[i]) {
            return NAME1_GREATER_THAN_NAME2;
        } else if (name1[i] < name2[i]) {
            return NAME2_GREATER_THAN_NAME1;
        }
    }
    return NAMES_ARE_SAME;
} /* End of name_comparator */
