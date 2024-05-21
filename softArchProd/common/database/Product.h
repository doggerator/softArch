#pragma once
#include <string>
#include <vector>
#include <optional>

#include <Poco/JSON/Object.h>

namespace database {

    class Product {
        private:
            long id_;
            std::string owner_id_;
            std::string name_;

        public:
            static void Initialize();

            static Product fromJSON(const std::string& str);
            static std::vector<Product> SelectAll();
            static std::vector<Product> SelectByOwnerId(std::string owner_id);


            inline long GetId() const { return id_; }
            std::string GetOwnerId() const { return owner_id_; }
            const std::string &GetName() const { return name_; }

            long& GetId() { return id_; }
            std::string& GetOwnerId() { return owner_id_; }
            std::string& GetName() { return name_; }
            void Save();

            Poco::JSON::Object::Ptr toJSON() const;

            friend std::ostream& operator<< (std::ostream& stream, const Product& product);
    };
}