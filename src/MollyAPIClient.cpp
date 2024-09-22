#include "MollyAPIClient.h" 
#include <iostream>

// Constructor: initializes the HTTP and WebSocket clients
MollyAPIClient::MollyAPIClient(const std::string& base_url, const std::string& ws_host, const std::string& ws_port)
    : http_client_(base_url), ws_client_(ioc_, ws_host, ws_port) {}

// Run method: handles login, WebSocket connection, and message processing
void MollyAPIClient::run(const std::string& username, const std::string& password)
{  
    try {
        // Step 1: Login and retrieve session token
        std::optional<std::string> token_opt = http_client_.login(username, password);
        
        if (!token_opt) {
            std::cerr << "Login failed. Unable to retrieve session token." << std::endl;
            return;  // Early exit if login fails
        }

        // Extract token from optional
        std::string token = *token_opt;
        std::cout << "Logged in successfully." << std::endl;
        //std::cout << "Logged in successfully. Token: " << token << std::endl;  // Comment this out for security

        // Step 2: Connect to the WebSocket using the token
        try {
            ws_client_.connect(token);
            std::cout << "Connected to WebSocket." << std::endl;
        } catch (const boost::system::system_error& e) {
            std::cerr << "WebSocket connection failed: " << e.what() << std::endl;
            return;  // Exit if WebSocket connection fails
        }

        // Step 3: Listen for messages until the "sync" message is received
        try {
            ws_client_.listen_until_sync();
            std::cout <<"--------------------demarcating the end of the message stream - message unique names-------------------------"<< std::endl;
            std::cout << "Successfully listened until sync message." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error during WebSocket message handling: " << e.what() << std::endl;
        }

    } catch (const std::exception& e) {
        // Catch any unforeseen exceptions
        std::cerr << "Unexpected error in MollyAPIClient::run: " << e.what() << std::endl;
    }
}
