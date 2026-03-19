#ifndef NODE_PUBLIC_H
#define NODE_PUBLIC_H


    #include <pthread.h>
    #include <unistd.h>

    // Data structure to define a node
    typedef struct node_t{
        //handlers
        int(*init_hdlr)(struct node_t *node);               // Initialize handler
        void(*send_hdlr)(struct node_t *node);               // Send handler 
        void(*recv_hdlr)(struct node_t *node);               // Receive handler
        void(*cleanup_hdlr)(struct node_t *node);           // Cleanup handler
        
        // socket tied to the node
        int sock;
        
        // buffers
        unsigned char *rcv_buffer;
        unsigned char *send_buffer;

        // Every node has a 64 bit NAME
        unsigned char name[8];
        
        // Source address 1 byte long
        unsigned char sa;

        // condtion wait for receiving thread
        pthread_mutex_t lock;
        pthread_cond_t recv_cond;

        // state info
        unsigned char state;

    } node_t;

#endif // NODE_PUBLIC_H