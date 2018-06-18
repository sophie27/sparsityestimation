CC=g++ 
CFLAGS=-I. -g -w -Wall 
all:
	$(CC) $(CFLAGS) nonoise.cpp -o gofree

.PHONY: opt
opt:override CFLAGS += -O2
opt:all

.PHONY: prof
prof:override CFLAGS += -p
prof:all

.PHONY: clean
clean:
	rm gofree
