
.PHONY: docker
docker:
	docker build -f ./.devcontainer/Dockerfile --tag=nullptr:latest \
		 --build-arg USE_CLANG=1 .
	docker run -it --rm -v $(PWD):/home/user/dev/ nullptr:latest
