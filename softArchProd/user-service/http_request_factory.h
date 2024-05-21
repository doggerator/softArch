#pragma once
#include <iostream>

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>

template <typename T>
class AuthRequired : public Poco::Net::HTTPRequestHandler {
private:
    T handler;

public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override {
        std::cout << "Run authentication ... " << std::endl;
        return handler.handleRequest(request, response);        
    }
};

class HTTPRequestFactory: public Poco::Net::HTTPRequestHandlerFactory {
private:
    std::string format_;

public:
    HTTPRequestFactory() = default;

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;
};
