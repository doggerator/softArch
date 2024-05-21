# Rest service ---- ---- ---- ---- ----

.PHONY: image
image:
	docker build --rm --no-cache -t uservice:hl .

.PHONY: image-build
image-build:
	docker build --rm --no-cache -t uservice-build:hl -f docker/Dockerfile_build .

.PHONY: build-user
build-user:
	cmake --build build --config Debug --target user-service -- -j4

.PHONY: run-user
run-user:
	./run-user.sh

# Other service ---- ---- ---- ---- ----

.PHONY: build-delivery
build-delivery:
	cmake --build build --config Debug --target delivery-service -- -j4

.PHONY: run-delivery
run-delivery:
	./run-delivery.sh

# Database ---- ---- ---- ---- ----

.PHONY: run-db
run-db:
	docker run --name database --detach --rm -p 6033:3306 \
		--env MYSQL_DATABASE=hl \
		--env MYSQL_USER=postgres \
		--env MYSQL_PASSWORD=postgres \
		--env MYSQL_ROOT_PASSWORD=postgres \
		mariadb:hl 

# Other service ---- ---- ---- ---- ----
.PHONY: up
up:
	docker compose up

.PHONY: down
down:
	docker compose down

