CC ?= cc
CFLAGS ?= -Wall -Wextra -Werror
LDFLAGS = -lutil

SRCS = main.c cmp.c print.c traverse.c util.c
OBJS = $(SRCS:.c=.o)
TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c ls.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) a.out *.core

.PHONY: all clean
