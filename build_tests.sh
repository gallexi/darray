gcc -ounit_tests -Wall -Wextra -std=c11 -g -I${EMU_ROOT} ./test/darray.test.c
g++ -operf_tests -w -fpermissive -std=c++14 -g ./test/perf.test.cpp
