#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <string>
#include <set>
#include <iostream>
#include <boost/asio/ssl.hpp>


namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

// Custom teardown for SSL streams
namespace boost {
namespace beast {
namespace websocket {

template<class Socket>
void teardown(
    beast::role_type role,
    net::ssl::stream<Socket>& stream,
    beast::error_code& ec)
{
    std::cout<<"teardown overload called"<<std::endl;
    stream.shutdown(ec);
}

} // namespace websocket
} // namespace beast
} // namespace boost

class WebSocketClient {
public:
   WebSocketClient(boost::asio::io_context& ioc, const std::string& host, const std::string& port)
    :     resolver_(ioc), 
          ssl_ctx(boost::asio::ssl::context::tlsv12_client), 
          ws_(ioc, ssl_ctx), 
          host_(host), 
          port_(port) 
    {
    ssl_ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3);
    ssl_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
}

    // Method to connect to the WebSocket using the session token
    void connect(const std::string& token);
    // Method to listen for messages until a "sync" message is received
    void listen_until_sync();
    

private:
    tcp::resolver resolver_; // Resolver for DNS lookup
    boost::asio::ssl::context ssl_ctx;
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> ws_;
    std::string host_; // Host for the WebSocket connection
    std::string port_; // Port for the WebSocket connection
};

#endif // WEBSOCKET_CLIENT_H
