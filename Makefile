BIN := bin

LIBS = -lz -lpthread -lprotobuf
CXX = g++ -std=c++11
CXXFLAGS = -g -Wall -O2 -ffast-math -I./ -D_GNU_SOURCE

ifeq ($(HW_SIMULATE), 1)
	CXXFLAGS += -DHW_SIMULATE=1
endif

ifeq ($(HWLINK_DEBUG), 1)
	CXXFLAGS += -DHWLINK_DEBUG=1
endif

.PHONY: default all clean protobuf

default: bin/main
all: default

PROTO = $(shell find ./protobuf -type f -name '*.proto')
OBJECTS = $(patsubst %.cpp, %.o, $(shell find . -type f -name '*.cpp'))
HEADERS = $(wildcard *.h)

.PRECIOUS: $(TARGET) $(OBJECTS)

protobuf:
	protoc --proto_path=./protobuf --cpp_out=./comm/pb $(PROTO)
	protoc -I=./protobuf/ --java_out=./debugclient/src/main/java/ $(PROTO)
	cd comm/pb && for f in *.cc; do mv "$$f" "`basename $$f .cc`.cpp"; done

bin/main: protobuf $(OBJECTS)
	$(CXX) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	find . -type f -name '*.o' -exec rm -f {} \;
	rm -f main
