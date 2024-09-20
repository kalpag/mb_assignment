#include "HTTPClient.h"  
#include <cpr/cpr.h>     
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

// Constructor initializing the base URL for the HTTP client
HTTPClient::HTTPClient(const std::string& base_url) : base_url_(base_url) {}

// Login method: sends a POST request and returns the session token
std::string HTTPClient::login(const std::string& username, const std::string& password)
{
  
    std::cout<<"HTTPClient::login"<<std::endl;
    // Use CPR to send a POST request for login
    cpr::Response response = cpr::Post(
        cpr::Url{base_url_ + "/v1/sessions"},
        cpr::Payload{{"username", username}, {"password", password}}
    );

    std::cout<<"response code = "<<response.status_code<<std::endl;

    if (response.status_code != 200) {
        throw std::runtime_error("Unfortunately Login failed: " + response.text);
    }
    
    auto json_response = nlohmann::json::parse(response.text);
    //std::cout << "Parsed JSON: " << json_response << std::endl;
    return json_response["data"];//token is sent in 'data'
}
