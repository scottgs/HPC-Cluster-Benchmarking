LIB_PATH = /usr/lib
LIBS = -lboost_system
CC = g++
CFLAGS = -Wall -O3 -L $(LIB_PATH)
OBJS = matrix.o
EXEC = mm

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

clean:
	rm *.o $(EXEC)