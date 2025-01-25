.PHONY: all clean build

all: build

build:
	@cd build && cmake ../ && make && clear

clean:
	@rm -rf build/*
	@echo "Cleaned build!"
