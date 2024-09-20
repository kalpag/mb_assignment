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
    // Resolve the DNS query to connect to the WebSocket server
    tcp::resolver::results_type results = resolver_.resolve(host_, port_);
    // Use the underlying TCP layer for connect
    boost::asio::connect(ws_.next_layer().next_layer(), results.begin(), results.end());
     // Perform SSL handshake on the SSL stream
    ws_.next_layer().handshake(boost::asio::ssl::stream_base::client);

    // Set the WebSocket handshake request
    ws_.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::request_type& req) {
            req.set(boost::beast::http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + "WebSocketClient");
        }
    ));

    // Perform WebSocket handshake
    boost::system::error_code ec;
    ws_.handshake(host_, "/v1/stream/?token=" + token, ec);

    if (!ec) {
        std::cout << "WebSocket handshake successful!" << std::endl;
    } 
    else{
        std::cerr << "WebSocket handshake failed: " << ec.message() << std::endl;
    }
}

// Listen for messages until the "sync" message is received
void WebSocketClient::listen_until_sync() 
{
    std::unordered_set<std::string> competition_names;  // Store distinct competition names
    boost::beast::flat_buffer buffer;  // Buffer to hold incoming messages
    
    try {
        for (;;) {  // Infinite loop until we receive a "sync" message
            ws_.read(buffer);  // red msg from WebSocket
            // Convert the buffer to a string and then to JSON
            std::string message = beast::buffers_to_string(buffer.data());
            nlohmann::json json_message = nlohmann::json::parse(message);

            // Check if it's an event message
            if (json_message.contains("data")) {
                for (const auto& data_item : json_message["data"]) {
                    if (data_item.is_array() && data_item.size() == 2) {
                        const std::string& message_type = data_item[0];

                        if (message_type == "event" && data_item[1].contains("competition_name")) {
                            std::string competition_name = data_item[1]["competition_name"].get<std::string>();
                            competition_names.insert(competition_name);
                        }
                        // Stop when the "sync" message is received
                        else if (message_type == "sync") {
                            std::cout << "Sync message received. Ending listener." << std::endl;
                            ws_.close(boost::beast::websocket::close_code::normal);//gracefully exit
                            
                            std::cout << "Distinct competition names received:" << std::endl;
                            for (const auto& name : competition_names){
                                std::cout << name << std::endl;
                            }

                            return;  // Exit the function once "sync" is received
                        }
                    }
                }
            }
            else {
                std::cerr << "Malformed message structure received, skipping." << std::endl;
                //should i log the malformed message here??
            }
            buffer.consume(buffer.size());  // Clear the buffer for the next message
        }
    }   catch (const boost::system::system_error& se) {
        std::cerr << "WebSocket error: " << se.what() << std::endl;
    } 
        catch (const nlohmann::json::exception& je) {
        std::cerr << "JSON parsing error: " << je.what() << std::endl;
    }
}
