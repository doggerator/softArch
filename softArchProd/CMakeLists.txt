cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (OTHER_SERVICE "other_service")

set(CMAKE_PREFIX_PATH
    "third_party/poco/cmake-build/package;third_party/redis-cpp/cmake-build/package"
)

find_package(redis-cpp)
find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${redis-cpp_INCLUDE_DIR})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")

link_directories("/usr/local/lib")
link_directories("third_party/poco/cmake-build/package/lib")
link_directories("third_party/redis-cpp/cmake-build/package/lib")


SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(common
    common/database/database.cpp
    common/database/User.cpp
    common/database/Product.cpp
    common/database/Delivery.cpp
    common/database/Cache.cpp
    common/config/config.cpp
)
target_compile_options(common PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(common PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(common PRIVATE 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${Poco_LIBRARIES}
    "PocoData"
    "PocoDataMySQL"
    "mysqlclient"
    ZLIB::ZLIB
    redis-cpp
)
include_directories(${CMAKE_SOURCE_DIR}/common)



add_executable(user-service
    user-service/main.cpp 
    user-service/http_request_factory.cpp
    user-service/user_handler.cpp
)
target_include_directories(user-service PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(user-service PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(user-service PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(user-service PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(user-service PRIVATE 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${Poco_LIBRARIES}
    "PocoData"
    "PocoDataMySQL"
    "mysqlclient"
    ZLIB::ZLIB
    common
)





add_executable(delivery-service
    delivery-service/main.cpp
    delivery-service/http_request_factory.cpp
    delivery-service/delivery_handler.cpp
    delivery-service/product_handler.cpp
)
target_include_directories(delivery-service PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(delivery-service PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(delivery-service PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(delivery-service PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_link_libraries(delivery-service PRIVATE 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${Poco_LIBRARIES}
    "PocoData"
    "PocoDataMySQL"
    "mysqlclient"
    ZLIB::ZLIB
    common
)
