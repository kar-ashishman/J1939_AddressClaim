#include<stdio.h>
#include <stdint.h>
#include "service.h"
#include "node_io_objs.h"
#include "service_defs.h"

int adress_table_update(address_table_entry_t new_entry){
    //update a table with the new address entry name and address 
    //This service is called only when a new address message received and there is no conflict with the address claimed by the node
    if(new_entry.address < 0 || new_entry.address > 253){
        return 0;
    }
    address_table_entry[new_entry.address].name = new_entry.name;
    address_table_entry[new_entry.address].address = new_entry.address;
    return 1;
}

int address_table_conflict_check(address_table_entry_t new_entry){
    if(new_entry.address <0 || new_entry.address > 253){
        //Address not applicable
        return ADDR_NOT_APPLICABLE;
    }
    else if(new_entry.name < address_table_entry[new_entry.address].name)
    {
        //update the address table with this name
        return UPDATE_ADDRESS_NAME;
    }
    else if(new_entry.name == address_table_entry[new_entry.address].name)
    {
        //do not update address table
        return DUPLICATE_NAME_ENTRY;
    }
}

int get_address_from_table(){
    return 0;
}
/*
    1. My NAME < Their NAME: Claim the address
    2. My NAME > Their NAME: Do not claim the address, wait and try again
    3. My NAME == Their NAME: Do not claim the address, wait and try again
    */