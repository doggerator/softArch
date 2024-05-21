export REST_SERVICE_HOST=localhost
export REST_SERVICE_PORT=8080

export DB_HOST=127.0.0.1
export DB_PORT=3306
export DB_LOGIN=postgres
export DB_PASSWORD=postgres
export DB_DATABASE=hl

export LD_LIBRARY_PATH=$(pwd)/third_party/poco/cmake-build/package/lib

./build/delivery-service
