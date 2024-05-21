#include <sstream>
#include <exception>

#include "Delivery.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <limits>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {

    void Delivery::Initialize() {
        std::cout << "[INFO] Creating Delivery table" << std::endl;
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);

            // Statement drop_stmt(session);
            // drop_stmt << "DROP TABLE IF EXISTS `Delivery`", now;
            // std::cout << "-- " << drop_stmt.toString() << std::endl;

            create_stmt << R"(
                CREATE TABLE IF NOT EXISTS `Delivery` (
                    `id` INT NOT NULL AUTO_INCREMENT,
                    `sender_id` VARCHAR(36) NOT NULL,
                    `reciever_id` VARCHAR(36) NOT NULL,
                    `product_id` INT NOT NULL,

                    FOREIGN KEY (`product_id`) REFERENCES Product(`id`),
                    PRIMARY KEY (`id`),
                    CONSTRAINT sender_not_equal_reciever CHECK (sender_id <> reciever_id)
                ); -- sharding:0
            )",now;
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

    Poco::JSON::Object::Ptr Delivery::toJSON() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", id_);
        root->set("recieverId", reciever_id_);
        root->set("senderId", sender_id_);
        root->set("productId", product_id_);

        return root;
    }

    Delivery Delivery::fromJSON(const std::string &str) {
        Delivery delivery;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        delivery.id_ = object->has("id") ? object->getValue<long>("id") : 0;
        delivery.sender_id_ = object->getValue<std::string>("senderId");
        delivery.reciever_id_ = object->getValue<std::string>("recieverId");
        delivery.product_id_ = object->getValue<long>("productId");

        return delivery;
    }

    std::vector<Delivery> Delivery::SelectAll() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Delivery> result;
            Delivery a;
            select << "SELECT id, sender_id, reciever_id, product_id FROM Delivery",
                into(a.id_),
                into(a.sender_id_),
                into(a.reciever_id_),
                into(a.product_id_),
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

    std::vector<Delivery> Delivery::Select(std::string sender_id, std::string reciever_id) {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Delivery> result;
            Delivery a;

            select << "SELECT id, sender_id, reciever_id, product_id FROM `Delivery`",
                into(a.id_), into(a.sender_id_), into(a.reciever_id_), into(a.product_id_);

            if (!sender_id.empty() || !reciever_id.empty()) {
                select << " WHERE ";
                if (!sender_id.empty())
                    select << "sender_id = ?" , use(sender_id);
                else
                    select << "1=1";

                select << " AND ";

                if (!reciever_id.empty())
                    select << "reciever_id = ?" , use(reciever_id);
                else
                    select << "1=1";
            }

            select << "",range(0, 1);

            std::cout << "-- " << select.toString() << std::endl;

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

    void Delivery::Save() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Delivery (sender_id, reciever_id, product_id) VALUES(?, ?, ?)",
                use(sender_id_),
                use(reciever_id_),
                use(product_id_);

            std::cout << "-- " << insert.toString() << std::endl;

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(id_),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done()) {
                select.execute();
            }
            std::cout << "[INFO] Inserted delivery " << id_ << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.displayText() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.message() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::MySQLException& e) {
            std::cout << "MySQLException:" << e.displayText() << std::endl;
            throw;
        }
    }

    std::ostream& operator<< (std::ostream& stream, const Delivery& delivery) {
        stream << "Delivery(id=" << delivery.id_
            << ", sender_id=" << delivery.sender_id_
            << ", reciever_id=" << delivery.reciever_id_
            << ", product_id=" << delivery.product_id_
            << ")";
        return stream;
    }

}