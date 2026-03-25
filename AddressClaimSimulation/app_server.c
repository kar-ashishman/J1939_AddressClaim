#include <stdio.h>
#include <unistd.h>
#include "node_public.h"
#include "node_defs.h"
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include "address_claim_proto.h"
#include "node_proto.h"

#define ADDRESS_LIST_MAX_SIZE 256
#define SA_NULL 254
#define SA_GLOBAL 255

node_t server = {
    .init_hdlr = node_init,
    .send_hdlr = node_send,
    .recv_hdlr = node_recv,
    .cleanup_hdlr = node_cleanup,
    .sock = INVALID_SOCKET_TYPE,
    .send_buffer = send_buffer,
    .rcv_buffer = recv_buffer,
};

//Each entry of the address table
typedef struct sa_entry = {
    unsigned char name[8];
    int sa;
    bool flag;
}sa_entry_t;

static sa_entry_t sa_table[ADDRESS_LIST_MAX_SIZE];

//Initialise all the entries to 0
void sa_entry_init(){
    for(int i=0;i<ADDRESS_LIST_MAX_SIZE;i++){
        memset(&sa_table[i],0,sizeof(sa_entry_t));
    }
}

//When message is received
void server_recv(node_t* node) {
    node->recv_hdlr(node);
    //Everytime you receive any address claim message, add into table
    //Need to do this only when you receive address claim message for the first time
    if(!sa_table[node->recv_buffer[2]].flag){
        sa_table[node->recv_buffer[2]].sa = node->recv_buffer[2];
        memcpy(sa_table[node->recv_buffer[2]].name,&node->recv_buffer[4],8);
        sa_table[node->recv_buffer[2]].flag = 1;
        break;
    }
    
    //Say the address is already stored - but you receive a address claim msg
    if(sa_table[node->recv_buffer[2]].flag && 
        (sa_table[node->recv_buffer[2]].sa ==  ode->recv_buffer[2]))
    {
        memcpy(sa_table[node->recv_buffer[2]].name,&node->recv_buffer[4],8);
    }

};

//Print address table 
void addr_table_print(){
    for(int i=0;i<ADDRESS_LIST_MAX_SIZE;i++){
        
    }
}





