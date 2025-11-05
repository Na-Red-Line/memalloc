CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -g -pthread
LDFLAGS = -pthread

TARGET = test_runner

$(TARGET): memalloc.c test.c
	$(CC) $(CFLAGS) -o $(TARGET) test.c $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o

test: run

.PHONY: run test clean
