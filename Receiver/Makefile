CC = g++
CFLAGS = -pthread -std=c++11 -Wall -Wextra -pedantic -O3
LDLIBS = -lSDL2 -lSDL2_image
SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main $(LDLIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main
