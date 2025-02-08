
.PHONY: docker
docker: image
	docker run -it --rm -v $(PWD):/home/dev/ nullptr:latest

.phony: image
image:
	docker build -f ./.devcontainer/Dockerfile --tag=nullptr:latest \
		 --build-arg USE_CLANG=1 .

.PHONY: cmake
cmake:
	cmake -B /home/dev/build -S /home/dev
	cmake . --preset unixlike-clang-debug
	${MAKE} compile

.PHONY: compile
compile:
	cmake --build /home/dev/build
