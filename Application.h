#pragma once

#include <iostream>
#include <map>
#include "GlobalDefs.h"
#include <mutex>

class Application {
private:
    std::mutex cli_mutex;
    
#if defined(_WIN32)
    std::map<std::string, std::string> color_map = {
        {"USEROPT", "Color 01"},
        {"ADDRCLM", "Color 0E"},
        {"CAN",     "Color 07"}
    };
#else
    std::map<std::string, std::string> color_map = {
        {"USEROPT", "\033[34m"},
        {"ADDRCLM", "\033[93m"},
        {"CAN",     "\033[37m"}
    };
#endif

    std::map<ac_id_t, ac_name_t> address_claim_table = {
        {0x10, 0x100},
        {0x20, 0x200},
        {0x30, 0x300}
    }; // Maps address to {Source Address, NAME}

public:
    template<typename T>
    void printf(T msg, std::string color);
    void useroptions();
    void print_address_claim_table();
    void claim_new_address();
    void signalnodes(ac_id_t dest_address, ac_name_t dest_name, int signal_type);
    void addr_claim_response();
};
