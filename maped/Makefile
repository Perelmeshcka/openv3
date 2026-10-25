CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -g -O3
LDFLAGS = -lm

# CFLAGS += $(shell pkg-config --cflags raylib)
# LDFLAGS += $(shell pkg-config --libs raylib)

CFLAGS += $(shell pkg-config --cflags raylib)
LDFLAGS += $(shell pkg-config --libs raylib)

TARGET = maped
SRCS = src/maped.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(OBJS): src/config.h

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJS)

.PHONY: all clean
