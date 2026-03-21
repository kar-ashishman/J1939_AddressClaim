#ifndef NODE_DEFS_H
#define NODE_DEFS_H

    #define PORT                    50000 
    #define GLOBALIP                "10.0.2.255"
    #define SENDER                  0
    #define RECEIVER                1
    #define BUFFER_LENGTH           13
    #define MAX_ATTEMPTS            5
    #define SOURCE_ADDRESS          0x01
    #define SECONDARY_ADDRESS       0x50
    #define SECONDARY_ADDRESS_END   0x5F
    #define TIMEOUT_MSECS           1250

    #define NODE_STATE_UNCLAIMED        0
    #define NODE_STATE_CLAIMING         1
    #define NODE_STATE_CLAIMED          2
    #define NODE_STATE_CONFICTED        3
    #define NODE_STATE_CLAIM_FAILED     4

    #define NAME1_GREATER_THAN_NAME2    1
    #define NAME2_GREATER_THAN_NAME1    2
    #define NAMES_ARE_SAME              0


    /*       DEFINES        */
    #define GET_ERROR() errno
    #define CLOSE_SOCKET close
    //#define SOCKET_TYPE int
    #define INVALID_SOCKET_TYPE -1
    #define SOCKET_ERROR_TYPE -1
    
#endif // NODE_DEFS_H