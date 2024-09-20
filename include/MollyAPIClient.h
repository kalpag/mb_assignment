#ifndef MOLLY_API_CLIENT_H
#define MOLLY_API_CLIENT_H

#include "HTTPClient.h"
#include "WebSocketClient.h"
#include <string>
#include <boost/asio.hpp>

class MollyAPIClient {
public:
    // Constructor with base URL for HTTP and host/port for WebSocket
    MollyAPIClient(const std::string& base_url, const std::string& ws_host, const std::string& ws_port);

    // Method to log in, connect to WebSocket, and process messages
    void run(const std::string& username, const std::string& password);

private:
    boost::asio::io_context ioc_; // Asynchronous I/O context
    HTTPClient http_client_;      // HTTP client for login
    WebSocketClient ws_client_;   // WebSocket client for message handling
};

#endif // MOLLY_API_CLIENT_H
