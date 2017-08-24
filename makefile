CC=gcc
CFLAGS=-g -Wall -Wextra -std=c11 -I${EMU_ROOT}
CPPC=g++
CPPFLAGS=-g -w -fpermissive -std=c++11

all: clean unit_tests perf_tests

unit_tests:
	@$(CC) $(CFLAGS) -ounit_tests ./test/darray.test.c

perf_tests:
	@$(CPPC) $(CPPFLAGS) -operf_tests ./test/perf.test.cpp

clean:
	@rm -f *.o unit_tests perf_tests
