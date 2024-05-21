export DB_HOST=127.0.0.1
export DB_PORT=6033
export DB_LOGIN=postgres
export DB_PASSWORD=postgres
export DB_DATABASE=hl

export USE_CACHE=1
export CACHE_HOST=localhost
export CACHE_PORT=6379

export LD_LIBRARY_PATH=$(pwd)/third_party/poco/cmake-build/package/lib

./build/user-service
