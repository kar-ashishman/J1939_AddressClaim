#include <thread>
#include "Application.h"
#include <iostream>
#include <unistd.h>

template<typename T>
void Application::printf(T msg, std::string color) {
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
                if (fork() == 0)
                    system("start cmd /k \"./mainprog\"");
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

