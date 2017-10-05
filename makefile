CC=gcc
CFLAGS=-Wall -Wextra -std=c11
CPPC=g++
CPPFLAGS=-g -w -fpermissive -std=c++11

BUILD_DIR=build/
TEST_DIR=test/
INSTALL_LIB_DIR=/usr/local/lib/
INSTALL_INCLUDE_DIR=/usr/local/include/

DARRAY_HEADER=darray.h
DARRAY_OBJ=darray.o
DARRAY_LIB=darray
DARRAY_LIB_OUT=lib$(DARRAY_LIB).a

all: build

build: clean
	mkdir -p $(BUILD_DIR) --mode=755
	gcc -c -o $(BUILD_DIR)$(DARRAY_OBJ) darray.c -O3
	ar rcs $(BUILD_DIR)$(DARRAY_LIB_OUT) $(BUILD_DIR)$(DARRAY_OBJ)

install: build
	install $(BUILD_DIR)$(DARRAY_LIB_OUT) $(INSTALL_LIB_DIR)$(DARRAY_LIB_OUT)
	install $(DARRAY_HEADER) $(INSTALL_INCLUDE_DIR)$(DARRAY_HEADER)

unit_tests: build
	$(CC) $(CFLAGS) -o $(BUILD_DIR)unit_tests $(TEST_DIR)darray.test.c -L$(BUILD_DIR) -I${EMU_ROOT} -l$(DARRAY_LIB)

perf_tests: build
	$(CPPC) $(CPPFLAGS) -o $(BUILD_DIR)perf_tests $(TEST_DIR)perf.test.cpp  -L$(BUILD_DIR) -l$(DARRAY_LIB) -O3

clean:
	@rm -rf $(BUILD_DIR)
