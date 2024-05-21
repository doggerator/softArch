#include <iostream>

#include <Poco/Net/HTMLForm.h>

#include <database/User.h>
#include <utils/utils.h>
#include "user_handler.h"

using Poco::Net::HTMLForm;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;

static bool hasSubstr(const std::string &str, const std::string &substr)
{
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i)
    {
        bool ok{true};
        for (size_t j = 0; ok && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}

bool check_name(const std::string &name, std::string &reason)
{
    if (name.length() < 3)
    {
        reason = "Name must be at leas 3 signs";
        return false;
    }

    if (name.find(' ') != std::string::npos)
    {
        reason = "Name can't contain spaces";
        return false;
    }

    if (name.find('\t') != std::string::npos)
    {
        reason = "Name can't contain spaces";
        return false;
    }

    return true;
}

bool check_email(const std::string &email, std::string &reason)
{
    if (email.find('@') == std::string::npos)
    {
        reason = "Email must contain @";
        return false;
    }

    if (email.find(' ') != std::string::npos)
    {
        reason = "EMail can't contain spaces";
        return false;
    }

    if (email.find('\t') != std::string::npos)
    {
        reason = "EMail can't contain spaces";
        return false;
    }

    return true;
}

Poco::JSON::Object::Ptr UserHandler::remove_password(Poco::JSON::Object::Ptr src) {
    if (src->has("password"))
        src->set("password", "*******");
    return src;
}

void UserHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
    HTMLForm form(request, request.stream());

    const auto method = request.getMethod();
    try {
        if (form.has("id") && (method == Poco::Net::HTTPRequest::HTTP_GET)) {
            const std::string& uuid = form.get("id");

            std::optional<database::User> result = database::User::SelectById(uuid);
            if (result) {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(remove_password(result->toJSON()), ostr);
                return;
            }
            else {
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/not_found");
                root->set("title", "Internal exception");
                root->set("status", "404");
                root->set("detail", "user ot found");
                root->set("instance", "/user");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
                return;
            }
        }
        else if (hasSubstr(request.getURI(), "/api/users/self") && (method == Poco::Net::HTTPRequest::HTTP_GET)) {

            std::string scheme;
            std::string info;
            request.getCredentials(scheme, info);

            std::string login, password;
            if (scheme == "Basic") {
                get_identity(info, login, password);
                if (auto user = database::User::Auth(login, password)) {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream& ostr = response.send();
                    Poco::JSON::Stringifier::stringify(remove_password(user->toJSON()), ostr);
                    return;
                }
            }

            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
            root->set("type", "/errors/unauthorized");
            root->set("title", "Internal exception");
            root->set("status", "401");
            root->set("detail", "not authorized");
            root->set("instance", "/auth");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(root, ostr);
            return;
        }
        else if (hasSubstr(request.getURI(), "/api/users/search") && (method == Poco::Net::HTTPRequest::HTTP_GET)) {
            std::string fn = form.get("first_name");
            std::string ln = form.get("last_name");
            auto results = database::User::search(fn, ln);
            Poco::JSON::Array arr;
            for (auto s : results)
                arr.add(remove_password(s.toJSON()));
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(arr, ostr);

            return;
        }
        else if (hasSubstr(request.getURI(), "/api/users/all") && (method == Poco::Net::HTTPRequest::HTTP_GET)) {
            auto results = database::User::read_all();
            Poco::JSON::Array arr;
            for (auto s : results)
                arr.add(remove_password(s.toJSON()));
            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
            response.setChunkedTransferEncoding(true);
            response.setContentType("application/json");
            std::ostream &ostr = response.send();
            Poco::JSON::Stringifier::stringify(arr, ostr);

            return;
        }
        else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
            if (form.has("first_name") && form.has("last_name") && form.has("email") && form.has("title") && form.has("login") && form.has("password")) {
                database::User user;
                user.first_name() = form.get("first_name");
                user.last_name() = form.get("last_name");
                user.email() = form.get("email");
                user.title() = form.get("title");
                user.login() = form.get("login");
                user.password() = form.get("password");

                bool check_result = true;
                std::string message;
                std::string reason;

                if (!check_name(user.get_first_name(), reason))
                {
                    check_result = false;
                    message += reason;
                    message += "<br>";
                }

                if (!check_name(user.get_last_name(), reason))
                {
                    check_result = false;
                    message += reason;
                    message += "<br>";
                }

                if (!check_email(user.get_email(), reason))
                {
                    check_result = false;
                    message += reason;
                    message += "<br>";
                }

                if (user.get_password().empty()) {
                    check_result = false;
                    message += "Password require";
                    message += "<br>";
                }

                if (check_result)
                {
                    user.save_to_mysql();
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");

                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(user.toJSON(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << message;
                    response.send();
                    return;
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << "[WARNING] Exception: " << e.what() << std::endl;
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    root->set("detail", "request ot found");
    root->set("instance", "/user");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}
