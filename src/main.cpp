#include "MollyAPIClient.h"
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>

int main(){
    std::cout << "Mollybet client starting..." << std::endl;

    try {
        // Step 1: Base URL for the Molly API
        constexpr char base_url[] = "https://api.mollybet.com";
        
        // Step 2: WebSocket host and port for Molly API
        constexpr char ws_host[] = "api.mollybet.com";
        constexpr char ws_port[] = "443";  // Secure WebSocket connection (wss)
        
        // Step 3: Load credentials (username and password) from environment variables
        const char* username = std::getenv("MOLLYBET_USERNAME");
        const char* password = std::getenv("MOLLYBET_PASSWORD");

        // Fall back to hardcoded values if environment variables are not set
        if (!username || !password) {
            username = "devinterview";
            password = "OwAb6wrocirEv";
            std::cerr << "Warning: Using hardcoded credentials for login." << std::endl;
        }

        // Step 4: Initialize Molly API Client with base URL, WebSocket host, and port
        MollyAPIClient molly_api_client(base_url, ws_host, ws_port);

        // Step 5: Run the client logic (login, connect to WebSocket, process messages)
        molly_api_client.run(username, password);
        
        std::cout << "Mollybet client finished successfully." << std::endl;
        return EXIT_SUCCESS;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return EXIT_FAILURE;  // Exit with a failure code if an exception is caught
    }
}
