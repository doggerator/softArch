#pragma once
#include <iostream>
#include <optional>

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/JSON/Object.h>

std::optional<Poco::JSON::Object::Ptr> AuthUser(const Poco::Net::HTTPServerRequest& request);

template <typename T>
class AuthRequired : public Poco::Net::HTTPRequestHandler {
private:
    T handler;

public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override {
        try {
            std::cout << "[INFO] Run authentication ..." << std::endl;
            auto user = AuthUser(request);
            if (!user.has_value())
                throw std::logic_error("wrong login/password");

            std::cout << "[INFO] Authentication success" << std::endl;
            return handler.handleRequest(request, response);
        }
        catch (const std::exception& e) {
            std::cout << "[WARNING] Auth exception: " << e.what() << std::endl;
        }
        catch (...) {
            std::cout << "[WARNING] Auth exception: unknown" << std::endl;
        }

        if (!response.sent()) {
            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            response.send();
        }
    }
};

class HTTPRequestFactory: public Poco::Net::HTTPRequestHandlerFactory {
private:
    std::string format_;

public:
    HTTPRequestFactory() = default;

    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;
};
