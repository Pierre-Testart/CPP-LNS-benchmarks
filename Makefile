CFLAGS=-std=c++14 -Wall -Wextra -pedantic -O3

SOURCES := main.cpp
OBJS := $(SOURCES:.cpp=.o)

benchmarks := FFT BLACKSCHOLES INVERSEK2J JMEINT SOBEL KMEANS
reference_type := float
benchmarked_type := lns32_t

benchmarks_flags := $(addprefix -DBENCHMARK_, $(benchmarks))

gcc: clean
gcc: CC=g++
gcc: make_exec

clang: clean
clang: CC=clang++
clang: make_exec

make_exec: $(OBJS)
	$(CC) $(CFLAGS) -o lns_benchmarks $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) $(benchmarks_flags) -DBENCHMARK_TYPE1="$(reference_type)" -DBENCHMARK_TYPE2="$(benchmarked_type)" -c $<

clean:
	rm -rf *.o lns_benchmarks
