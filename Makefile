SRC := countlets.cpp klets.cpp shuffler.cpp shuffle_euler.cpp \
  shuffle_linear.cpp shuffle_markov.cpp
OBJ_COUNTLETS := countlets.o klets.o
OBJ_SHUFFLER := shuffler.o klets.o shuffle_euler.o shuffle_linear.o \
  shuffle_markov.o
CC := g++

all: build install

build:
	cd src;\
	$(CC) --std=c++11 -O2 -Wall -c $(SRC)

countlets:
	cd src;\
	$(CC) $(OBJ_COUNTLETS) -o ../bin/countlets

shuffler:
	cd src;\
	$(CC) $(OBJ_SHUFFLER) -o ../bin/shuffler

makebin:
	mkdir -p bin

install: makebin countlets shuffler

clean:
	cd src;\
	rm -f *.o
