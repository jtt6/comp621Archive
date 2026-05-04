CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -g -I./include
LDFLAGS = -lpthread -lnsl -lrt
PROGS = matmul_O2 matmul_transpose_O2 matmul_block_O2 matmul_O3 matmul_block_O3
DATA = matrix1000.dat matrix2000.dat

all: progs data

progs: $(PROGS)

data: $(DATA)

$(DATA): matrices.tar.xz
	tar -xvf matrices.tar.xz

matmul_O2: main.c matmul.c csapp.o
	$(CC) $(CFLAGS) -O2 -o matmul_O2 main.c matmul.c csapp.o $(LDFLAGS)

matmul_transpose_O2: main.c matmul_transpose.c csapp.o
	$(CC) $(CFLAGS) -O2 -o matmul_transpose_O2 main.c matmul_transpose.c csapp.o $(LDFLAGS)

matmul_block_O2: main.c matmul_block.c csapp.o
	$(CC) $(CFLAGS) -O2 -o matmul_block_O2 main.c matmul_block.c csapp.o $(LDFLAGS)

#matmul_O2_floop-block: main.c matmul.c csapp.o
#	$(CC) $(CFLAGS) -O2 -floop-block -o matmul_O2_floop-block main.c matmul.c csapp.o $(LDFLAGS)

matmul_O3: main.c matmul.c csapp.o
	$(CC) $(CFLAGS) -ffast-math -O3 -o matmul_O3 main.c matmul.c csapp.o $(LDFLAGS)

matmul_block_O3: main.c matmul_block.c csapp.o
	$(CC) $(CFLAGS) -ffast-math -O3 -o matmul_block_O3 main.c matmul_block.c csapp.o $(LDFLAGS)

#matmul_block_O3_floop-block: main.c matmul_block.c csapp.o
#	$(CC) $(CFLAGS) -ffast-math -O3 -floop-block -o matmul_block_O3_floop-block main.c matmul_block.c csapp.o $(LDFLAGS)

csapp.o: ./include/csapp.c ./include/csapp.h
	$(CC) $(CFLAGS) -c ./include/csapp.c -o csapp.o

clean: clean-progs
	$(RM) *.dat

clean-progs:
	$(RM) *.o $(PROGS) core.[1-9]*

.PHONY: all progs data clean clean-progs
