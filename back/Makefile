MAKEFLAGS += --always-make # required for proper function 
export CMAKE_EXPORT_COMPILE_COMMANDS ?= 1 # export compile commands for code analysis
export CMAKE_LOG_LEVEL ?= NOTICE


### config ###

THREADS ?= $(shell nproc)
BUILD_PATH ?= ./build


### targets ###

all: run

build: 
	cmake -B $(BUILD_PATH)
	cmake --build $(BUILD_PATH) -j $(THREADS)

run: build
	@ echo 
	@ $(BUILD_PATH)/main

clean:
ifdef BUILD_PATH
	rm -rf $(BUILD_PATH)/*
endif
