#include <iostream>
#include <thread>
#include "Application.h"

int main() {
    system("clear");
    Application app;
    app.printf("Ashish", "ADDRCLM");
    std::thread app_thread(&Application::useroptions, &app);
	app_thread.join();
}