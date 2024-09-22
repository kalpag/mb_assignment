#include "WebSocketClient.h" 
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include<boost/asio/connect.hpp>
#include <iostream>
#include <iterator>
#include <string>
#include <nlohmann/json.hpp>
#include <unordered_set>

using namespace std;

// Connect to the WebSocket server using the session token
void WebSocketClient::connect(const std::string& token) 
{
    // Lambda to log errors if any occur
    auto log_error = [](const std::string& context, const boost::system::error_code& ec) {
        if (ec) {
            std::cerr << context << " WebSocketClient::connect failed: " << ec.message() << std::endl;
        }
    };

    boost::system::error_code ec;

    // Resolve the DNS query to connect to the WebSocket server
    tcp::resolver::results_type results = resolver_.resolve(host_, port_, ec);
    log_error("DNS resolution", ec);
    if (ec) return;

    // Use the underlying TCP layer to connect
    boost::asio::connect(ws_.next_layer().next_layer(), results.begin(), results.end(), ec);
    log_error("TCP connection", ec);
    if (ec) return;

    // Perform SSL handshake on the SSL stream
    ws_.next_layer().handshake(boost::asio::ssl::stream_base::client, ec);
    log_error("SSL handshake", ec);
    if (ec) return;

    // Set the WebSocket handshake request
    ws_.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req) {
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " WebSocketClient");
        }
    ));

    // Perform WebSocket handshake
    ws_.handshake(host_, "/v1/stream/?token=" + token, ec);
    log_error("WebSocket handshake", ec);
    if (ec) return;

    std::cout << "WebSocket handshake successful!" << std::endl;
}

// Listen for messages until the "sync" message is received
void WebSocketClient::listen_until_sync() 
{
    std::unordered_set<std::string> competition_names;  // Store distinct competition names
    boost::beast::flat_buffer buffer;  // Buffer to hold incoming messages
    
    // Lambda for graceful WebSocket closure
    auto close_connection = [&]() {
        ws_.close(boost::beast::websocket::close_code::normal);
        std::cout << "Distinct competition names received:" << std::endl;
        std::cout <<"--------------------demarcating the start of the message stream - message unique names-------------------------"<< std::endl;
        for (const auto& name : competition_names) {
            std::cout << name << std::endl;
        }
    };

    try {
        while (true) {  // Loop until we receive a "sync" message
            ws_.read(buffer);  // Read message from WebSocket

            const std::string message = beast::buffers_to_string(buffer.data());  // Convert buffer to string
            nlohmann::json json_message;
            
            try {
                json_message = nlohmann::json::parse(message);  
            } catch (const nlohmann::json::exception& je) {
                std::cerr << "JSON parsing error: " << je.what() << ". Skipping message: " << message << std::endl;
                buffer.consume(buffer.size());  // Clear buffer for next message
                continue;  // Skip to next message on parsing failure
            }

            // Check if it's an event message
            if (json_message.contains("data")) {
                for (const auto& data_item : json_message["data"]) {
                    if (data_item.is_array() && data_item.size() == 2) {
                        const std::string& message_type = data_item[0];

                        if (message_type == "event" && data_item[1].contains("competition_name")) {
                            const std::string competition_name = data_item[1]["competition_name"].get<std::string>();
                            competition_names.insert(competition_name);
                        }
                        // Stop when the "sync" message is received
                        else if (message_type == "sync") {
                            std::cout << "Sync message received. Ending listener." << std::endl;
                            close_connection();  // Gracefully exit
                            return;  // Exit the function
                        }
                    }
                }
            } else {
                std::cerr << "Malformed message structure received: " << message << std::endl;
            }

            buffer.consume(buffer.size());  // Clear buffer for the next message
        }
    } catch (const boost::system::system_error& se) {
        std::cerr << "WebSocket error: " << se.what() << std::endl;
        close_connection();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        close_connection();
    }
}

