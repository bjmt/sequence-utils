SRC := countlets.cpp klets.cpp shuffler.cpp shuffle_linear.cpp shuffle_markov.cpp
# OBJ := $(addsuffix .o, $(basename $(SRC)))
CC  := g++

all: build install

build:
	cd src;\
	$(CC) --std=c++11 -O2 -c $(SRC)

countlets:
	cd src;\
	$(CC) countlets.o klets.o -o ../bin/countlets

shuffler:
	cd src;\
	$(CC) shuffler.o klets.o shuffle_linear.o shuffle_markov.o -o ../bin/shuffler

makebin:
	mkdir -p bin

install: makebin countlets shuffler

clean:
	cd src;\
	rm -f *.o
