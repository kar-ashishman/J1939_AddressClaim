#pragma once

#include <iostream>
#include <map>

class Application {
private:
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

    std::map<int, unsigned long long> address_claim_table = {
        {0x10,0x100},
        {0x20, 0x200},
        {0x30, 0x300}
    }; // Maps address to {Source Address, NAME}

public:
    template<typename T>
    void printf(T msg, std::string color);
    
    void useroptions();
    void print_address_claim_table();
};
