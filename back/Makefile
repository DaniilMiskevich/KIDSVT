MAKEFLAGS += --always-make # required for proper function 


### config ###

THREADS ?= $(shell nproc)
THREADS ?= 1
BUILD_PATH ?= ./build


### targets ###

all: run

build: 
ifeq ($(OS), Windows_NT)
	@ set CMAKE_EXPORT_COMPILE_COMMANDS=1 && cmake -B $(BUILD_PATH) 
else 
	@ CMAKE_EXPORT_COMPILE_COMMANDS=1 cmake -B $(BUILD_PATH) 
endif
	@ cmake --build $(BUILD_PATH) -j $(THREADS)

run: build
	@ echo 
	@ $(BUILD_PATH)/main

clean:
ifdef BUILD_PATH
ifeq ($(OS), Windows_NT)
	rd /q /s $(BUILD_PATH)/*
else
	rm -rf $(BUILD_PATH)/*
endif
endif
