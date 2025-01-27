.PHONY: all clean build

all: build

build:
	@cd build && cmake ../ && make

clean:
	@rm -rf build/*
	@echo "Cleaned build!"
