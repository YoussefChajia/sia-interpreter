.PHONY: all
all: build

.PHONY: build
build:
	@cd build && make && cmake --build . && clear
