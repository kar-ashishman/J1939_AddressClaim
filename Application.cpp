#include <thread>
#include "Application.h"
#include "GlobalDefs.h"
#include <iostream>
#include <chrono>

#ifdef _WIN32
    #include <windows.h>
#else   
    #include <unistd.h>
#endif 

template<typename T>
void Application::printf(T msg, std::string color) {
    std::lock_guard<std::mutex> lock(cli_mutex);
    #if defined(_WIN32)
        system(color_map[color]);
        std::cout << msg;
        system("Color 07"); // Reset to default color
    #else // For Unix-like systems
        std::cout << color_map[color] << msg << "\033[0m";
    #endif
}

void Application::print_address_claim_table() {
    printf("Address Claim Table:\n", "ADDRCLM");
    printf("ID \t\t NAME\n", "ADDRCLM");

    for (const auto& [key, value] : address_claim_table) {
        printf(key, "ADDRCLM");
        printf(" \t\t ", "ADDRCLM");
        printf(value, "ADDRCLM");
        std::cout << std::endl;
    }
}

void Application::claim_new_address() {
    // Prompt for new name and ID
    int temp = 0; // use this variable to fetch id. as cin with ac_id_t causes issues
    ac_id_t id = 0;
    ac_name_t name = 0;
    printf("Enter new Source Address - ", "USEROPT");
    std::cin >> temp;
    id = temp;
    printf("Enter new name - ", "USEROPT");
    std::cin >> name;
    // Find NAME in address_claim_table
    bool name_exists = false;
    for (const auto& [key, value] : address_claim_table) {
        if (value == name) {
            name_exists = true;
            break;
        }
    }
    if(name_exists) {
        // check if address is already claimed with this NAME
        if(address_claim_table[name] == id) {
            printf("Address already claimed with this NAME.\n", "CAN");
        }
        else {
            // send signal to change NAME
            signalnodes(id, name, (int)SIG_CLAIM_ADDR);
            // wait for response in a detached thread
            std::thread t (&Application::addr_claim_response, this);
            t.detach();
        }
    }

    else {
        // create a new node 
        //signal it to claim new address
        signalnodes(id, name, (int)SIG_CLAIM_ADDR);
        // wait for response in a detached thread
        std::thread t (&Application::addr_claim_response, this);
        t.detach();
    }
}

void Application::useroptions() {
    /*
       1. Print Address Claim Table
       2. Claim a new Address
       3. Clear screen
    */
   int opt;
   while(1) {
        printf("======================================\n", "USEROPT");
		printf("1. Print Address Claim Table\n2. Claim a new Address\n3. Clear screen\n",
            "USEROPT");
        printf("======================================\n", "USEROPT");
        printf("Enter your option: ", "USEROPT");
		std::cin >> opt;
        std::cout << std::endl;
		
        switch(opt) {
            case 1:
                print_address_claim_table();
                break;
            case 2:
                claim_new_address();
                break;
            case 3:
                #if defined(_WIN32)
                    system("cls");
                #else
                    system("clear");
                #endif
                break;
        }
   	// execute adequate callbacks	
	}
}

void Application::signalnodes(ac_id_t dest_address, ac_name_t dest_name, int signal_type) {
    
    if (signal_type == SIG_CLAIM_ADDR) {
        printf("Claiming address for name - ", "CAN");
        printf(dest_name, "CAN");
        printf(" with Source ID - ", "CAN");
        printf(static_cast<int>(dest_address), "CAN");
        printf("\n", "CAN");
        // Send signal to dest_address to claim address and wait for a response

    }
    else if (signal_type == SIG_CHANGE_NAME) {
        // Send signal to dest_address to change NAME to dest_name
    }
}

void Application::addr_claim_response() {
    using namespace std::chrono;
    auto start = steady_clock::now();
    while(duration_cast<milliseconds>(steady_clock::now() - start).count() < 5000) {
    }
    printf("End of operation\n", "CAN");
}