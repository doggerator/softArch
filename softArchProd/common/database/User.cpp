#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/UUIDGenerator.h>

#include <sstream>
#include <exception>

#include "User.h"
#include "database.h"
#include "../config/config.h"
#include "Cache.h"

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try {
            Poco::Data::Session session = database::Database::get().create_session();

            for (const auto& hint : database::Database::GetAllHints()) {
                Statement create_stmt(session);

                // Statement drop_stmt(session);
                // drop_stmt << "DROP TABLE IF EXISTS `User`" << hint, now;
                // std::cout << "-- " << drop_stmt.toString() << std::endl;

                create_stmt << R"(
                    CREATE TABLE IF NOT EXISTS `User` (
                        `id` INT NOT NULL AUTO_INCREMENT,
                        `uuid` VARCHAR(36) NOT NULL,
                        `first_name` VARCHAR(256) NOT NULL,
                        `last_name` VARCHAR(256)  NOT NULL,
                        `login` VARCHAR(256)      NOT NULL,
                        `password` VARCHAR(256)   NOT NULL,
                        `email` VARCHAR(256)      NULL,
                        `title` VARCHAR(1024)     NULL,
                        PRIMARY KEY (`id`),
                        KEY `fn` (`first_name`),
                        KEY `ln` (`last_name`),
                        KEY (`uuid`),
                        KEY (`login`)
                    )
                )" << hint, now;

                std::cout << "-- " << create_stmt.toString() << std::endl;

                std::cout << "[INFO] Create user database instance" << std::endl;
            }

        }
        catch (Poco::Data::MySQL::ConnectionException& e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException& e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _uuid);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);
        root->set("login", _login);

        return root;
    }

    User User::fromJSON(const std::string& str) {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        std::cout << str << std::endl;
        user.uuid() = object->getValue<std::string>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.title() = object->getValue<std::string>("title");
        user.login() = object->getValue<std::string>("login");

        if (object->has("password"))
            user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<User> User::Auth(std::string& login, std::string& password) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User a;

            for (const auto& hint : Database::GetAllHints()) {
                Poco::Data::Statement select(session);
                select << "SELECT id, uuid, first_name, last_name, email, title, login FROM User WHERE login = ? and password = ?" + hint,
                    into(a._id),
                    into(a._uuid),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    use(login),
                    use(password),
                    range(0, 1); //  iterate over result set one row at a time

                std::cout << "-- " << select.toString() << std::endl;

                select.execute();
                Poco::Data::RecordSet rs(select);
                if (rs.moveFirst())
                    return a;
            }
        }
        catch (Poco::Data::MySQL::ConnectionException& e) {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException& e) {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::optional<User> User::SelectById(std::string uuid) {
        if (Config::get().get_use_cache()) {
            auto user = User::GetFromCache(uuid);
            if (user.has_value()) {
                // std::cout << "[DEBUG] Using cached user " << user->get_uuid() << std::endl;
                return user;
            }
                // std::cout << "[DEBUG] Cache miss for " << uuid << std::endl;
        }

        try {
            Poco::Data::Session session = database::Database::get().create_session();
            User a;

            Poco::Data::Statement select(session);
            select << "SELECT id, uuid, first_name, last_name, email, title,login,password FROM User where uuid=?" + Database::GetShardingHint(uuid),
                into(a._id),
                into(a._uuid),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                into(a._login),
                into(a._password),
                use(uuid),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) {
                if (Config::get().get_use_cache()) {
                    a.SaveToCache();
                    // std::cout << "[DEBUG] Cache user " << a._uuid << std::endl;
                }
                return a;
            }
        }
        catch (Poco::Data::MySQL::ConnectionException& e) {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException& e) {
            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::vector<User> User::read_all() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
            User a;

            for (const auto& hint : Database::GetAllHints()) {
                Statement select(session);
                select << "SELECT id, uuid, first_name, last_name, email, title, login, password FROM User" + hint,
                    into(a._id),
                    into(a._uuid),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    range(0, 1); //  iterate over result set one row at a time

                std::cout << "-- " << select.toString() << std::endl;

                while (!select.done()) {
                    if (select.execute())
                        result.push_back(a);
                }
            }

            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException& e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException& e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::MySQLException& e) {
            std::cout << "MySQLException: " << e.displayText() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(std::string first_name, std::string last_name) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
            User a;
            first_name += "%";
            last_name += "%";

            for (const auto& hint : Database::GetAllHints()) {
                Statement select(session);
                select << "SELECT id, uuid, first_name, last_name, email, title, login, password FROM User where first_name LIKE ? and last_name LIKE ?" + hint,
                    into(a._id),
                    into(a._uuid),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    use(first_name),
                    use(last_name),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done()) {
                    if (select.execute())
                        result.push_back(a);
                }
            }
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::string User::GenerateUUID() {
        static Poco::UUIDGenerator generator = Poco::UUIDGenerator();
        return generator.create().toString();
    }

    void User::save_to_mysql() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            // Read about it in `README.md`
            auto uuid = GenerateUUID();
            insert << "INSERT INTO User (uuid,first_name,last_name,email,title,login,password) VALUES(?, ?, ?, ?, ?, ?, ?)" + Database::GetShardingHint(uuid),
                use(uuid),
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_title),
                use(_login),
                use(_password);

            std::cout << "-- " << insert.toString() << std::endl;

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()" + Database::GetShardingHint(uuid),
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            std::cout << "-- " << select.toString() << std::endl;

            if (!select.done()) {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::MySQLException& e) {
            std::cout << "MySQLException: " << e.displayText() << std::endl;
            throw;
        }
    }

    void User::SaveToCache() const {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::Instance().Put(_uuid, message);
    }

    std::optional<User> User::GetFromCache(const std::string& id) {
        try {
            std::string result;
            if (database::Cache::Instance().Get(id, result))
                return fromJSON(result);
            else
                return std::optional<User>();
        }
        catch (const std::exception& e) {
            std::cout << "cache error: " << e.what() << std::endl;
            return std::optional<User>();
        }
    }

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    const std::string& User::get_uuid() const {
        return _uuid;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_email() const
    {
        return _email;
    }

    const std::string &User::get_title() const
    {
        return _title;
    }

    long &User::id()
    {
        return _id;
    }

    std::string& User::uuid() {
        return _uuid;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::title()
    {
        return _title;
    }
}