CC = gcc
CCFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -O0
INCLUDE = -Iinclude

DOCKER_CUSTOM_IMAGE_NAME = my_local_images/doxygen

CURRENT_DIRECTORY = $(shell pwd)
CURRENT_LIBRARY_VERSION = $(shell cat VERSION)

.DEFAULT_GOAL := help

build_directory:
	mkdir -p build

example_build_demo: \
	build_directory \
	include/fancy_memory.h \
	src/fancy_memory.c \
	examples/demo.c
	$(CC) $(CCFLAGS) $(INCLUDE) \
	src/fancy_memory.c \
	examples/demo.c \
	-o build/demo_using_all_methods_and_types

example_run_demo: example_build_demo
	./build/demo_using_all_methods_and_types

test_build_unit: \
	build_directory \
	include/fancy_memory.h \
	src/fancy_memory.c \
	test/main.c
	$(CC) $(CCFLAGS) $(INCLUDE) -DRUN_INTEGRATION_TEST=0 \
	src/fancy_memory.c \
	test/main.c \
	-o build/test_unit

test_run_unit: test_build_unit
	./build/test_unit

test_build_integration: \
	build_directory \
	include/fancy_memory.h \
	src/fancy_memory.c \
	test/main.c
	$(CC) $(CCFLAGS) $(INCLUDE) -DRUN_INTEGRATION_TEST=1 \
	src/fancy_memory.c \
	test/main.c \
	-o build/test_integration

test_run_integration: test_build_integration
	./build/test_integration

.PHONY: doxygen_build doxygen_build_for_docs_website help

doxygen_build:
	docker build -f doxygen/Dockerfile -t $(DOCKER_CUSTOM_IMAGE_NAME) .
	docker run \
    	--rm \
    	-v $(CURRENT_DIRECTORY):/my-dir \
    	-w /my-dir \
    	$(DOCKER_CUSTOM_IMAGE_NAME) doxygen doxygen/Doxyfile

doxygen_build_for_docs_website:
	docker build -f doxygen/Dockerfile -t $(DOCKER_CUSTOM_IMAGE_NAME) .
	docker run \
		--rm \
		-v $(CURRENT_DIRECTORY):/my-dir \
		-v $(CURRENT_DIRECTORY)/../c-fancy-memory-docs/docs/v$(CURRENT_LIBRARY_VERSION):/my-dir/build-doxygen \
		-w /my-dir \
		$(DOCKER_CUSTOM_IMAGE_NAME) doxygen doxygen/Doxyfile

help:
	@echo "\nmake example_build_demo\n\tBuilds the 'demo.c' example."
	@echo "\nmake example_run_demo\n\tRuns the 'demo.c' example."
	@echo "\nmake test_build_unit\n\tBuilds the unit test."
	@echo "\nmake test_run_unit\n\tRuns the unit test."
	@echo "\nmake test_build_integration\n\tBuilds the integration test."
	@echo "\nmake test_run_integration\n\tRuns the integration test."
	@echo "\nmake doxygen_build\n\tBuilds the Doxygen website using Docker and outputs the result into './build-doxygen'."
	@echo "\nmake doxygen_build_for_docs_website\n\tBuilds the Doxygen website using Docker and outputs the result into '../c-fancy-memory-docs/docs/v$(CURRENT_LIBRARY_VERSION)'."
	@echo ""