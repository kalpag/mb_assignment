#include "MollyAPIClient.h"
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <string>


int main() 
{
    std::cout<<"mollybet client starting....."<<std::endl;
    try {
        // Base URL for the Molly API
        const std::string base_url = "https://api.mollybet.com";
        
        // WebSocket host and port for Molly API
        const std::string ws_host = "api.mollybet.com";
        const std::string ws_port = "443"; //secure WebSocket connection (wss)
        
        // Credentials (for the test environment provided)
        const std::string username = "devinterview";
        const std::string password = "OwAb6wrocirEv";

        // Initialize Molly API Client
        MollyAPIClient molly_api_client(base_url, ws_host, ws_port);

        // Run the login, connect to WebSocket, and process messages
        molly_api_client.run(username, password);
    } catch (const std::exception& e) {
        std::cerr << "Error: Main Method : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
