SRC := countlets.cpp shuffler.cpp shuffle_linear.cpp shuffle_markov.cpp
# OBJ := $(addsuffix .o, $(basename $(SRC)))
CC  := g++

all: build install

build:
	cd src;\
	$(CC) --std=c++11 -O2 -c $(SRC)

install:
	mkdir -p bin;\
	cd src;\
	$(CC) shuffler.o shuffle_linear.o shuffle_markov.o -o ../bin/shuffler;\
	$(CC) countlets.o shuffle_markov.o -o ../bin/countlets

clean:
	cd src;\
	rm -f *.o
