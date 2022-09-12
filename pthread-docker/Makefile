all: build

build:
	gcc -o test -pthread test.c

docker-build: build
	docker build -t pthread-docker:latest .

docker-run: docker-build
	docker run --rm -ti pthread-docker:latest

docker-run-elevated: docker-build
	docker run --rm -ti --cap-add sys_nice pthread-docker:latest
