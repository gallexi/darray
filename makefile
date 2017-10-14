CC=gcc
CFLAGS=-Wall -Wextra -std=c11
CPPC=g++
CPPFLAGS=-Wall -Wextra -std=c++11

BUILD_DIR=build/
TEST_DIR=test/
INSTALL_LIB_DIR=/usr/local/lib/
INSTALL_INCLUDE_DIR=/usr/local/include/

DARRAY_HEADER=darray.h
DSTRING_HEADER=dstring.h
DARRAY_OBJ=darray.o
DARRAY_LIB=darray
DARRAY_LIB_OUT=lib$(DARRAY_LIB).a

OPTIMIZATION_LEVEL=-O3

all: build unit_tests perf_tests

build: clean
	mkdir -p $(BUILD_DIR) --mode=755
	gcc $(CFLAGS) -c -o $(BUILD_DIR)$(DARRAY_OBJ) darray.c $(OPTIMIZATION_LEVEL)
	ar rcs $(BUILD_DIR)$(DARRAY_LIB_OUT) $(BUILD_DIR)$(DARRAY_OBJ)

install: build
	install $(BUILD_DIR)$(DARRAY_LIB_OUT) $(INSTALL_LIB_DIR)$(DARRAY_LIB_OUT)
	install $(DARRAY_HEADER) $(INSTALL_INCLUDE_DIR)$(DARRAY_HEADER)
	install $(DSTRING_HEADER) $(INSTALL_INCLUDE_DIR)$(DSTRING_HEADER)

unit_tests: build
	$(CC) $(CFLAGS) -o $(BUILD_DIR)unit_tests $(TEST_DIR)darray.test.c -L$(BUILD_DIR) -I${EMU_ROOT} -l$(DARRAY_LIB)

perf_tests: build
	$(CC) $(CFLAGS) -o $(BUILD_DIR)perf_tests_darr $(TEST_DIR)perf_tests/perf.test.c -L$(BUILD_DIR) -l$(DARRAY_LIB) $(OPTIMIZATION_LEVEL)
	$(CPPC) $(CPPFLAGS) -o $(BUILD_DIR)perf_tests_vector $(TEST_DIR)perf_tests/perf.test.cpp $(OPTIMIZATION_LEVEL)

clean:
	@rm -rf $(BUILD_DIR)
