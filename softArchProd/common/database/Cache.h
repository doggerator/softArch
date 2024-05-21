#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <mutex>

namespace database {

    class Cache {
        private:
            std::shared_ptr<std::iostream> stream_;
            std::mutex mut_;

            Cache();
        public:
            static Cache& Instance();

            void Put(const std::string& id, const std::string& value);
            bool Get(const std::string& id, std::string& value);
    };
}
