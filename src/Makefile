CC=gcc
CXX=g++

all: gen-ktest load-ktest

gen-ktest: gen-ktest.cpp
	$(CXX) gen-ktest.cpp KTest.cpp -o gen-ktest
	
load-ktest: load-ktest.cpp
	$(CXX) load-ktest.cpp KTest.cpp -o load-ktest	
