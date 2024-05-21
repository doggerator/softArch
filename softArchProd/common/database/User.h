#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class User{
    private:
        long _id;
        std::string _first_name;
        std::string _last_name;
        std::string _email;
        std::string _title;
        std::string _login;
        std::string _password;
        std::string _uuid;

    public:

        static User fromJSON(const std::string & str);

        long             get_id() const;
        const std::string& get_first_name() const;
        const std::string& get_last_name() const;
        const std::string& get_email() const;
        const std::string& get_title() const;
        const std::string& get_login() const;
        const std::string& get_password() const;
        const std::string& get_uuid() const;

        long&        id();
        std::string& first_name();
        std::string& last_name();
        std::string& email();
        std::string& title();
        std::string& login();
        std::string& password();
        std::string& uuid();

        static void init();
        static std::vector<User> read_all();
        static std::vector<User> search(std::string first_name,std::string last_name);

        static std::optional<User> SelectById(std::string uuid);
        static std::optional<User> Auth(std::string &login, std::string &password);
        void save_to_mysql();
        
        void SaveToCache() const;
        static std::optional<User> GetFromCache(const std::string& id);

        Poco::JSON::Object::Ptr toJSON() const;

        static std::string GenerateUUID();
    };
}

#endif