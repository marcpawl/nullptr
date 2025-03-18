
.PHONY: docker
docker: image
	-docker run  --rm -v $(PWD):/home/dev/ nullptr:latest make cmake
	docker run --name nullptr_latest -it --rm -v $(PWD):/home/dev/ nullptr:latest

.phony: image
image:
	docker build -f ./.devcontainer/Dockerfile --tag=nullptr:latest \
		 --build-arg USE_CLANG=1 .

.PHONY: cmake
cmake:
	git submodule init
	git submodule update
	cmake -B /home/dev/build -S /home/dev -G Ninja -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
	cmake . --preset unixlike-clang-debug
	${MAKE} compile

.PHONY: compile
compile: /home/dev/build/Release/generators/CMakePresets.json /home/dev/CMakeUserPresets.json
	ninja -C /home/dev/build
	ninja -C /home/dev/build test
	/home/dev/build/benchmarks/benchmarks

/home/dev/.conan2/profiles/default:
	conan profile detect

/home/dev/build/Release/generators/CMakePresets.json /home/dev/CMakeUserPresets.json &: /home/dev/.conan2/profiles/default
	conan install . --build=missing

.PHONY: format
format:
	ninja -C /home/dev/build clangformat

