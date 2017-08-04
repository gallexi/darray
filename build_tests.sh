gcc -ounit_tests -g -Wall -Wextra -std=c11 -I${EMU_ROOT} ./test/darray.test.c
g++ -operf_tests -g -w -fpermissive ./test/perf.test.cpp
