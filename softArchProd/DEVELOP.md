

Poco
----

```bash
# Install MySQL deps
sudo apt install mysql-client libmysqlclient-dev

cd third_party
git clone -b master https://github.com/pocoproject/poco.git
cd poco
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_DATA_MYSQL=ON -B cmake-build
cmake --build cmake-build -- -j$(nproc)
cmake --install cmake-build --prefix cmake-build/package


cd third_party
git clone https://github.com/tdv/redis-cpp.git
cd redis-cpp
cmake -DCMAKE_BUILD_TYPE=Release -B cmake-build
cmake --build cmake-build -- -j$(nproc)
cmake --install cmake-build --prefix cmake-build/package
```




Database
----

```bash
# Build image in proj root folder
docker build -t mariadb:hl --no-cache -f docker/mmysql/Dockerfile docker/mysql/

# Run container
docker run --name database --rm -p 3306:3306 \
    --env MYSQL_DATABASE=hl \
    --env MYSQL_USER=postgres \
    --env MYSQL_PASSWORD=postgres \
    --env MYSQL_ROOT_PASSWORD=postgres \
    mariadb:hl

# SQL Shell
mysql -h 127.0.0.1 --password -u postgres hl
```

