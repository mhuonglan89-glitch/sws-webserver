CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude
TARGET = sws
OBJS = src/main.o src/http.o src/response.o

all: $(TARGET)

$(TARGET): $(OBJS)
$(CC) $(CFLAGS) -o $@ $(OBJS)

src/%.o: src/%.c include/sws.h
$(CC) $(CFLAGS) -c $< -o $@

clean:
rm -f src/*.o $(TARGET) access.log

.PHONY: all clean
