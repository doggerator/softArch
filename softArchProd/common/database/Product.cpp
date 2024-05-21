#include <sstream>
#include <exception>

#include "Product.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {

    void Product::Initialize() {
        std::cout << "[INFO] Creating Product table" << std::endl;
        try {
            Poco::Data::Session session = database::Database::get().create_session();

            // Statement drop1(session);
            // drop1 << "DROP TABLE IF EXISTS `Delivery`", now;
            // std::cout << "-- " << drop1.toString() << std::endl;
            // Statement drop2(session);
            // drop2 << "DROP TABLE IF EXISTS `Product`", now;
            // std::cout << "-- " << drop2.toString() << std::endl;

            Statement create_stmt(session);
            create_stmt << R"(
                CREATE TABLE IF NOT EXISTS `Product` (
                    `id` INT NOT NULL AUTO_INCREMENT,
                    `owner_id` VARCHAR(36) NOT NULL,
                    `name` VARCHAR(256) NOT NULL,

                    PRIMARY KEY (`id`)
                ); -- sharding:0
            )",now;

            std::cout << "-- " << create_stmt.toString() << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException& e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException& e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
        catch (...) {
            std::cout << "Exception: " << std::endl;
        }
    }
    
    Poco::JSON::Object::Ptr Product::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", id_);
        root->set("ownerId", owner_id_);
        root->set("name", name_);

        return root;
    }

    Product Product::fromJSON(const std::string &str) {
        Product product;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        product.id_ = object->has("id") ? object->getValue<long>("id") : 0;
        product.owner_id_ = object->getValue<std::string>("ownerId");
        product.name_ = object->getValue<std::string>("name");

        return product;
    }

    std::vector<Product> Product::SelectAll() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Product> result;
            Product a;
            select << "SELECT id, owner_id, name FROM Product",
                into(a.id_),
                into(a.owner_id_),
                into(a.name_),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
                if (select.execute())
                    result.push_back(a);
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<Product> Product::SelectByOwnerId(std::string owner_id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Product> result;
            Product a;

            select << "SELECT id, owner_id, name FROM Product where owner_id = ?",
                into(a.id_),
                into(a.owner_id_),
                into(a.name_),
                use(owner_id),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
                if (select.execute())
                    result.push_back(a);
            return result;
        }
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Product::Save() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Product (owner_id, name) VALUES(?, ?)",
                use(owner_id_),
                use(name_);

            std::cout << "-- " << insert.toString() << std::endl;

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(id_),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done()) {
                select.execute();
            }
            std::cout << "[INFO] Inserted product " << id_ << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::MySQLException& e) {
            std::cout << "MySQLException:" << e.displayText() << std::endl;
            throw;
        }
    }

    std::ostream& operator<< (std::ostream& stream, const Product& product) {
        stream << "Product(id=" << product.id_ << ", name=" << product.name_ << ", owner_id=" << product.owner_id_ << ")";
        return stream;
    }

}