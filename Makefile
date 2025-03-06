.PHONY: all clean build test

all: build test

build:
	@cd build && cmake ../ && make

test:
	@cd build && ./sia_test --output-on-failure

clean:
	@rm -rf build/*
	@echo "Cleaned build!"
