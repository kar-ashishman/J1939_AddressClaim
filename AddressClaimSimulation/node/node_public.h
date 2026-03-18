#ifndef NODE_PUBLIC_H
#define NODE_PUBLIC_H


    #include<pthread.h>

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
        char *rcv_buffer;
        char *send_buffer;

        // Every node has a 64 bit NAME
        unsigned char name[8];
        
        // Source address 1 byte long
        unsigned char sa;
        
        //mutex lock for the node
        pthread_mutex_t mutex_lock;

        //condition waiting
        pthread_cond_t cond_wait;

        //type of node sender or receiver
        unsigned char type;
    } node_t;

#endif // NODE_PUBLIC_H