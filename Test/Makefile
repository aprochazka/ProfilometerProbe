CC = gcc
CFLAGS = -Wall -Wextra

SRCS = uart_jpeg.c
OBJS = $(SRCS:.c=.o)

all: uart_jpeg

uart_jpeg: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) uart_jpeg