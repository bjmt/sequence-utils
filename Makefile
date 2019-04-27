OBJ_COUNTLETS = countlets.o klets.o
OBJ_SHUFFLER = shuffler.o klets.o shuffle_euler.o shuffle_linear.o shuffle_markov.o
OBJ_SEQGEN = seqgen.o
OBJ_COUNTFA = countfa.o

CXX = g++
CXXFLAGS += --std=c++11 -O3 -Wall

all: build install

build:
	cd src;\
	$(CXX) $(CXXFLAGS) -c *.cpp

countfa:
	  $(CXX) -o bin/countfa $(addprefix src/, $(OBJ_COUNTFA))

countlets:
	$(CXX) -o bin/countlets $(addprefix src/, $(OBJ_COUNTLETS))

shuffler:
	$(CXX) -o bin/shuffler $(addprefix src/, $(OBJ_SHUFFLER))

seqgen:
	$(CXX) -o bin/seqgen $(addprefix src/, $(OBJ_SEQGEN))

makebin:
	mkdir -p bin

install: makebin countfa countlets shuffler seqgen

clean:
	cd src;\
	rm -f *.o
