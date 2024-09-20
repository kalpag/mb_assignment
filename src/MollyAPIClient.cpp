#include "MollyAPIClient.h" 
#include <iostream>

// Constructor: initializes the HTTP and WebSocket clients
MollyAPIClient::MollyAPIClient(const std::string& base_url, const std::string& ws_host, const std::string& ws_port)
    : http_client_(base_url), ws_client_(ioc_, ws_host, ws_port) {}

// Run method: handles login, WebSocket connection, and message processing
void MollyAPIClient::run(const std::string& username, const std::string& password)
{  
    try{
         // Step 1: Login and retrieve session token
        std::string token = http_client_.login(username, password);
        std::cout << "Logged in successfully. Token: " << token << std::endl;// remember to comment this: FFS

        // Step 2: Connect to the WebSocket using the token
        ws_client_.connect(token);

        // Step 3: Listen for messages until the "sync" message is received
        ws_client_.listen_until_sync();
    } 
    catch (const std::exception& e) {
        std::cerr << "Error in MollyAPIClient: " << e.what() << std::endl;
    }
}
