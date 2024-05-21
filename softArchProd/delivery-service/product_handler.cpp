#include <iostream>
#include <Poco/Net/HTMLForm.h>

#include <database/Product.h>
#include "product_handler.h"

using Poco::Net::HTMLForm;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

void ProductHandler::handleRequest(HTTPServerRequest &request, [[maybe_unused]] HTTPServerResponse &response) {

    try {
        // Create product from JSON body
        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST && request.getContentType() == "application/json") {
            std::string body(std::istreambuf_iterator<char>(request.stream()), {});
            auto product = database::Product::fromJSON(body);

            std::cout << "[INFO] Inserting " << product << std::endl;

            product.Save();

            std::cout << "[INFO] Product: " << product << std::endl;

            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");

            auto& out = response.send();
            Poco::JSON::Stringifier::stringify(product.toJSON(), out);
            return;
        }
        else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            HTMLForm form(request, request.stream());


            std::vector<database::Product> products;
            if (form.has("ownerId")) {
                std::string owner_id = form.get("ownerId");
                products = database::Product::SelectByOwnerId(std::move(owner_id));
            }
            else {
                products = database::Product::SelectAll();
            }

            auto result = Poco::JSON::Array();
            for (auto product : products)
                result.add(product.toJSON());
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setContentType("application/json");
            auto& out = response.send();
            Poco::JSON::Stringifier::stringify(result, out);
            return;
        }
    }
    catch (const std::exception& e) {
        std::cout << "[ERROR] ProductHandler::handleRequest: " << e.what() << std::endl;
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        response.send();
        return;
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_FORBIDDEN);
    response.send();
}

