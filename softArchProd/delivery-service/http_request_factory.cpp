#include <iostream>

#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPSClientSession.h>

#include <utils/utils.h>

#include "product_handler.h"
#include "delivery_handler.h"

#include "http_request_factory.h"

using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPRequestHandler;

static bool StartsWith(const std::string &str, const std::string &substr) {
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i) {
        bool ok{true};
        for (size_t j = 0; ok && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}


std::optional<std::string> GetRequest(const std::string &url, const std::string &login, const std::string &password) {
    std::string string_result;
    try {
        std::string token = login + ":" + password;
        std::ostringstream os;
        Poco::Base64Encoder b64in(os);
        b64in << token;
        b64in.close();
        std::string identity = "Basic " + os.str();

        Poco::URI uri(url);
        Poco::Net::HTTPClientSession s(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
        request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
        request.setContentType("application/json");
        request.set("Authorization", identity);
        request.set("Accept", "application/json");
        request.setKeepAlive(true);
        s.sendRequest(request);

        Poco::Net::HTTPResponse response;
        std::istream &rs = s.receiveResponse(response);

        while (rs) {
            char c{};
            rs.read(&c, 1);
            if (rs)
                string_result += c;
        }

        if (response.getStatus() != 200)
            return {};
    }
    catch (Poco::Exception& ex) {
        std::cout << "exception:" << ex.what() << std::endl;
        return std::optional<std::string>();
    }

    return string_result;
}

std::optional<Poco::JSON::Object::Ptr> AuthUser(const HTTPServerRequest& request) {
    std::string scheme, info;
    std::string login, password;

    request.getCredentials(scheme, info);
    if (scheme == "Basic") {
        get_identity(info, login, password);
        std::cout << "login:" << login << std::endl;
        std::cout << "password:" << password << std::endl;
        std::string host = "localhost";
        std::string url;

        if (std::getenv("SERVICE_HOST") != nullptr)
            host = std::getenv("SERVICE_HOST");
        url = "http://" + host + ":8080/api/users/self";

        if (auto user = GetRequest(url, login, password); user.has_value()) {
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse(user.value());
            return result.extract<Poco::JSON::Object::Ptr>();
        }
    }

    return std::optional<Poco::JSON::Object::Ptr>();
}



HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request) {

    std::cout << request.getMethod() << " " << request.getURI()<< std::endl;

    // Create product => POST /api/products + JSON
    // Get user's products => GET /api/products?&userId=0
    // Create delivery => POST /api/delivery/ + JSON
    // Get delivery info => GET /api/delivery?senderId=0&recieverId=0

    if (StartsWith(request.getURI(), "/api/products"))
        return new AuthRequired<ProductHandler>();

    if (StartsWith(request.getURI(), "/api/deliveries"))
        return new AuthRequired<DeliveryHandler>();

    return 0;
}
