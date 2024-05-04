CC = g++
CFLAGS = -Wall -g
LIBS = -lglfw -lGL -lm

SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

EXEC = main

.PHONY: all clean run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC) $(OBJS)
