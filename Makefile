CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=
SOURCES=base64.cpp main.cpp server.cpp websockets.cpp chat.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=mokou

.PHONY: all clean

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: $(SOURCES)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)