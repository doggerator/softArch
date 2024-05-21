#pragma once
#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Object.h>

namespace database {

    class Delivery {
        private:
            long id_;
            std::string sender_id_;
            std::string reciever_id_;
            long product_id_;

        public:
            static void Initialize();

            static Delivery fromJSON(const std::string& str);
            static std::vector<Delivery> SelectAll();
            static std::vector<Delivery> Select(std::string sender_id = {}, std::string reciever_id = {});

            inline long GetId() const { return id_; }
            std::string GetSenderId() const { return sender_id_; }
            std::string GetRecieverId() const { return reciever_id_; }

            long& GetId() { return id_; }
            std::string& GetSenderId() { return sender_id_; }
            std::string& GetRecieverId() { return reciever_id_; }

            void Save();
            Poco::JSON::Object::Ptr toJSON() const;

            friend std::ostream& operator<< (std::ostream& stream, const Delivery& product);
    };
}