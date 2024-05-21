#include "Cache.h"
#include "../config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>


namespace database {

    Cache::Cache() {
        std::string host = Config::get().get_cache_host();
        std::string port = Config::get().get_cache_port();

        stream_ = rediscpp::make_stream(host, port);
    }

    Cache& Cache::Instance() {
        static Cache instance;
        return instance;
    }

    void Cache::Put(const std::string& id, const std::string& value) {
        std::lock_guard<std::mutex> lck(mut_);
        rediscpp::value response = rediscpp::execute(
            *stream_, "set", id, value, "ex", "60"
        );
    }

    bool Cache::Get(const std::string& id, std::string& value) {
        std::lock_guard<std::mutex> lck(mut_);
        rediscpp::value response = rediscpp::execute(*stream_, "get", id);

        if (response.is_error_message())
            return false;
        if (response.empty())
            return false;

        value = response.as<std::string>();
        return true;
    }
}
