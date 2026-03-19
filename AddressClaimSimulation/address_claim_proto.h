#ifndef ADDRESS_CLAIM_PROTO_H
#define ADDRESS_CLAIM_PROTO_H
    #include "node_public.h"

    void address_claim(node_t* node);
    void address_claim_parser(node_t* node);
    int name_comparator(unsigned char* name1, unsigned char* name2);
    
#endif // ADDRESS_CLAIM_PROTO_H