#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

class HTTPClient {
public:
    HTTPClient(const std::string& base_url);

    // Method to login and return a session token
    std::string login(const std::string& username, const std::string& password);

private:
    std::string base_url_; // Base URL for API requests
};

#endif // HTTP_CLIENT_H
