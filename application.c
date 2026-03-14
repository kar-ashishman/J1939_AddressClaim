#include <stdio.h>
#include "node_defs.h"
#include "node_public.h"
#include "node_proto.h"

#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <pthread.h>
#endif

char node_send_buffer[BUFFLEN];
char node_rcv_buffer[BUFFLEN];

node_t node_tx = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_revc,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .rcv_buffer = NULL,
    .send_buffer = node_send_buffer,
    .type = SENDER
};

node_t node_rx = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_revc,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET,
    .rcv_buffer = node_rcv_buffer,
    .send_buffer = NULL,
    .type = RECEIVER
};

/* Thread function to receive address claim messages */
#ifdef _WIN32
unsigned __stdcall receive_thread(void *arg) {
#else
void *receive_thread(void *arg) {
#endif
    node_t *rx_node = (node_t *)arg;
    
    printf("Receiver thread started, waiting for address claim...\n");
    
    /* Initialize the receiver node */
    if (rx_node->init_hdlr(rx_node) != 0) {
        printf("Failed to initialize receiver node\n");
        #ifdef _WIN32
            return 1;
        #else
            return NULL;
        #endif
    }
    
    /* Wait for incoming address claim message */
    if (rx_node->recv_hdlr(rx_node) == 0) {
        printf("Received address claim: %s\n", rx_node->rcv_buffer);
    }
    
    /* Cleanup */
    rx_node->cleanup_hdlr(rx_node);
    
    #ifdef _WIN32
        return 0;
    #else
        return NULL;
    #endif
}

int main() {
#ifdef _WIN32
    HANDLE hThread;
    unsigned threadId;
#else
    pthread_t thread;
#endif

    /* Initialize the sender node */
    if (node_tx.init_hdlr(&node_tx) != 0) {
        return 1;
    }

    /* Spawn receiver thread BEFORE sending (so it's ready to receive) */
#ifdef _WIN32
    hThread = (HANDLE)_beginthreadex(NULL, 0, receive_thread, &node_rx, 0, &threadId);
    if (hThread == NULL) {
        printf("Failed to create receiver thread\n");
        node_tx.cleanup_hdlr(&node_tx);
        return 1;
    }
#else
    if (pthread_create(&thread, NULL, receive_thread, &node_rx) != 0) {
        printf("Failed to create receiver thread\n");
        node_tx.cleanup_hdlr(&node_tx);
        return 1;
    }
#endif

    /* Give receiver thread time to initialize and start listening */
    #ifdef _WIN32
        Sleep(100);  /* 100 milliseconds */
    #else
        usleep(100000);  /* 100 milliseconds */
    #endif

    /* Send address claim message */
    printf("Sending address claim message...\n");
    snprintf(node_tx.send_buffer, BUFFLEN, "ADDRESS_CLAIM");
    node_tx.send_hdlr(&node_tx);

    /* Wait for receiver thread to complete */
#ifdef _WIN32
    WaitForSingleObject(hThread, 5000);  /* Wait up to 5 seconds */
    CloseHandle(hThread);
#else
    pthread_join(thread, NULL);
#endif

    /* Cleanup sender */
    node_tx.cleanup_hdlr(&node_tx);

    printf("Address claim process complete.\n");
    return 0;
}