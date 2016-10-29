CC=g++
CPPFLAGS = -std=c++11 -Wall
TARGET = example

all: $(TARGET)

$(TARGET): $(TARGET).cc
	$(CC) $(CPPFLAGS) -o $(TARGET) $(TARGET).cc

clean:
	rm example
