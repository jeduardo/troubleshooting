all: docker-run


run:
	./test.lua

docker-build:
	docker build -t lualanes-docker:latest .

docker-run: docker-build
	docker run --rm -ti lualanes-docker:latest

docker-run-privileged: docker-build
	docker run --rm -ti --cap-add sys_nice lualanes-docker:latest
