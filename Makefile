BIN := bin

LIBS = -lz -lpthread
CXX = g++ -std=c++11
CXXFLAGS = -g -Wall -O2 -ffast-math -I./ -D_GNU_SOURCE

.PHONY: default all clean

default: bin/main
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(shell find . -type f -name '*.cpp'))
HEADERS = $(wildcard *.h)

.PRECIOUS: $(TARGET) $(OBJECTS)

bin/main: $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	find . -type f -name '*.o' -exec rm -f {} \;
	rm -f main
