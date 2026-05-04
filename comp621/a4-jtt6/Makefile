all: io-static io-dynamic

io-static: io.c
	gcc -Wall -o $@ -DPROGNAME=\"$@\" -D_GNU_SOURCE io.c -static
io-dynamic: io.c
	gcc -Wall -o $@ -DPROGNAME=\"$@\" -D_GNU_SOURCE io.c
