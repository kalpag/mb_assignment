#include "HTTPClient.h"  
#include <cpr/cpr.h>     
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

// Constructor initializing the base URL for the HTTP client
HTTPClient::HTTPClient(const std::string& base_url) : base_url_(base_url) {}

std::optional<std::string> HTTPClient::login(const std::string& username, const std::string& password)
{
    std::cout << "Attempting to login with username: " << username << std::endl;
    
    // Send POST request for login using CPR
    cpr::Response response = cpr::Post(
        cpr::Url{base_url_ + "/v1/sessions"},
        cpr::Payload{{"username", username}, {"password", password}}
    );
    
    // Log response details for debugging purposes
    std::cout << "HTTP status code: " << response.status_code << std::endl;
    //std::cout << "Response text: " << response.text << std::endl; // commenting out as this exposes the token
    
    if (response.status_code != 200) {
        std::cerr << "Login failed with status code: " << response.status_code 
                  << " and message: " << response.text << std::endl;
        return std::nullopt;  
    }

    try {
        // Parse JSON response
        auto json_response = nlohmann::json::parse(response.text);
        
        if (json_response.contains("data")) {
            // Return token if available
            return json_response["data"].get<std::string>();
        } else {
            std::cerr << "Login response does not contain 'data' field." << std::endl;
            return std::nullopt;  
        }
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return std::nullopt;  
    }
}
