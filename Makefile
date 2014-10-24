LIBS = `pkg-config --libs opencv` -ljpeg
CXX = g++ -std=c++11 -I./
CFLAGS = -g -Wall

.PHONY: default all clean

default: main
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(shell find . -type f -name '*.cpp'))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

main: $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	find . -type f -name '*.o' -exec rm -f {} \;
	rm -f main
