#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_defs.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "address_claim_proto.h"
#include "node_proto.h"

void address_claim(node_t* node) {
    // updating network address table
    //node->send_buffer[2] = 0xFE;
    //node->send_hdlr(node);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    #if TIMEOUT_MSECS >= 1000
        ts.tv_sec += TIMEOUT_MSECS / 1000;
        ts.tv_nsec += (TIMEOUT_MSECS % 1000) * 1000000;
    #else
        ts.tv_nsec += TIMEOUT_MSECS * 1000000;
    #endif

    while(1) {
        // wait for a signal from receiving thread to start claiming address
        // This happens in two cases:
        // 1. When the node is initialized and is ready to claim address
        // 2. When the node receives a message with same SA as its own SA,
        //    which means there is an address conflict and it needs to claim a new address
        pthread_mutex_lock(&node->lock1);
        pthread_cond_wait(&node->recv_cond1, &node->lock1);
        pthread_mutex_unlock(&node->lock1);

        // try to claim address by sending addr claim message
        // Wait for a signal from rcving thread 
        // Timeout of 10 seconds for receiving a message
        // After timeout, claim the address
        while(node->state != NODE_STATE_CLAIMED) {
            node->send_hdlr(node);
            node->state = NODE_STATE_CLAIMING;
            int rc = pthread_cond_timedwait(&node->recv_cond2, &node->lock2, &ts);
            if (rc == ETIMEDOUT) {
                // address claim successful
                // update address table
                // unlock mutex and break
                node->state = NODE_STATE_CLAIMED;
                address_table_update(node);
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
                node->send_buffer[2] = node->sa;
            }
        }
    }
} /* End of address_claim */


void address_claim_parser(node_t* node) {
    int result;

    // signal sender thread to start claiming address
    pthread_mutex_lock(&node->lock1);
    pthread_cond_signal(&node->recv_cond1);
    pthread_mutex_unlock(&node->lock1);

    while(1) {
        // Wait for incoming messages and parse them
        printf("wating for a message\n");
        node->recv_hdlr(node);

        // Check if name of the incoming message is same as the node's name
        result = name_comparator(&(node->rcv_buffer[4]), 
                node->name);
        if (result == NAMES_ARE_SAME)
            continue; // Ignore messages from self

        // if incoming message is from a different SA
        if(node->rcv_buffer[2] != node->sa) {
            // update address table
            address_table_update(node);
        } else {
            // if incoming message is from same SA
            // address conflict
            // verify name superiority
            if(result == NAME1_GREATER_THAN_NAME2)
                node->send_hdlr(node);
            else {
                // higher priority node has claimed the address, so this node needs to claim a new address
                // signal sender thread to claim new address
                printf("Doing address mod\n");
                node->sa = node->sa == SOURCE_ADDRESS ? 
                        SECONDARY_ADDRESS : node->sa+1;
                node->send_buffer[2] = node->sa;

                if(node->state == NODE_STATE_CLAIMED) {
                    pthread_mutex_lock(&node->lock1);
                    pthread_cond_signal(&node->recv_cond1);
                    pthread_mutex_unlock(&node->lock1);
                } else {
                    pthread_mutex_lock(&node->lock2);
                    pthread_cond_signal(&node->recv_cond2);
                    pthread_mutex_unlock(&node->lock2);
                }
                node->state = NODE_STATE_CLAIMING;
            }
        }
    }

} /* End of address_claim_parser */

void address_table_update(node_t* node) {
    for(int i=0; i<8; i++) {
        node->table[node->rcv_buffer[2]][i] = node->rcv_buffer[i+4];
    }
    node->table[node->rcv_buffer[2]][8] = 1;
    node_table_printer(node);
} /* End of address_table_update */

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
