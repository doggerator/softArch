#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/JSON/Object.h>

class UserHandler : public Poco::Net::HTTPRequestHandler {
public:
    UserHandler(const std::string &format) : _format(format) { }

    Poco::JSON::Object::Ptr remove_password(Poco::JSON::Object::Ptr src);
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
    std::string _format;
};
