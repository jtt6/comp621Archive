CC       = gcc
CFLAGS   = -std=c17 -Wall -Wextra -Werror -g

OPT      = 3
PERF_STACK = 6

PROGRAMS = sample-nonopt sample-opt sample-gprof-nonopt sample-gprof-opt sample-gcov

.PHONY: all all-profiling perf-out

all: ${PROGRAMS}

perf-out: perf-nonopt.out perf-opt.out

all-profiling: gmon-nonopt.out gmon-opt.out sample.c.gcov perf-out

sample-nonopt: sample.c
	${CC} ${CFLAGS} -static -o sample-nonopt sample.c

sample-opt: sample.c
	${CC} ${CFLAGS} -O${OPT} -static -o sample-opt sample.c

sample-gprof-nonopt: sample.c
	${CC} ${CFLAGS} -pg -static -o sample-gprof-nonopt sample.c

sample-gprof-opt: sample.c
	${CC} ${CFLAGS} -pg -O${OPT} -static -o sample-gprof-opt sample.c

gmon-nonopt.out: sample-gprof-nonopt testinput
	./sample-gprof-nonopt < testinput
	gprof sample-gprof-nonopt > gmon-nonopt.out

gmon-opt.out: sample-gprof-opt testinput
	./sample-gprof-opt < testinput
	gprof sample-gprof-opt > gmon-opt.out

perf-opt.out: sample-opt
	perf record -e cycles:u --call-graph fp ./$< < testinput
	perf report -s sym,srcline --stdio --max-stack ${PERF_STACK} > $@

perf-nonopt.out: sample-nonopt
	perf record -e cycles:u --call-graph fp ./$< < testinput
	perf report -s sym,srcline --stdio --max-stack ${PERF_STACK} > $@

sample-gcov: sample.c
	${CC} ${CFLAGS} -fprofile-arcs -ftest-coverage -o sample-gcov sample.c

sample.c.gcov: sample-gcov
	./sample-gcov < testinput
	gcov sample.c

clean:
	${RM} ${PROGRAMS} *.out *.gcov *.gcno *.gcda core.[1-9]* perf.data*
