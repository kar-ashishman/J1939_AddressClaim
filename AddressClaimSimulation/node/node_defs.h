#ifndef NODE_DEFS_H
#define NODE_DEFS_H

    #define PORT                50000 
    #define GLOBALIP            "172.17.255.255"
    #define BUFFLEN             20
    #define SENDER              0
    #define RECEIVER            1
    #define BUFFER_LENGTH       100


    /*       DEFINES        */
    #define GET_ERROR() errno
    #define CLOSE_SOCKET close
    //#define SOCKET_TYPE int
    #define INVALID_SOCKET_TYPE -1
    #define SOCKET_ERROR_TYPE -1
    
#endif // NODE_DEFS_H