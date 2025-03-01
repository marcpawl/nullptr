
.PHONY: docker
docker: image
	docker run -it --rm -v $(PWD):/home/dev/ nullptr:latest

.phony: image
image:
	docker build -f ./.devcontainer/Dockerfile --tag=nullptr:latest \
		 --build-arg USE_CLANG=1 .

.PHONY: cmake
cmake:
	cmake -B /home/dev/build -S /home/dev -G Ninja -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
	cmake . --preset unixlike-clang-debug
	${MAKE} compile

.PHONY: compile
compile: /home/dev/.conan2/profiles/default
	conan install . --build=missing
	ninja -C /home/dev/build
	ninja -C /home/dev/build test
	/home/dev/build/benchmarks/benchmarks

/home/dev/.conan2/profiles/default:
	conan profile detect

.PHONY: format
format:
	ninja -C /home/dev/build clangformat

