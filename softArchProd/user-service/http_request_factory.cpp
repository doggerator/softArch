#include <iostream>

#include "user_handler.h"
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

HTTPRequestHandler* HTTPRequestFactory::createRequestHandler(const HTTPServerRequest& request) {

    std::cout << request.getMethod() << " " << request.getURI()<< std::endl;
    if (StartsWith(request.getURI(),"/api/users") ||
        StartsWith(request.getURI(),"/api/users/search") ||
        StartsWith(request.getURI(),"/api/users/all") ||
        StartsWith(request.getURI(),"/api/users/self")) 
        return new UserHandler(format_);

    return 0;
}
