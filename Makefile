CXX = g++
CXXFLAGS =-Wall -std=c++11
#Makefile exapmle
TARGET=main
CC=g++
FLAGLIB=-lminisat
CFLAGS=-g -Wall
OBJECTS=main.o command.o 
#don't change this
all:$(TARGET)
clean:
	rm -f *.o $(OBJECTS) core
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(FLAGLIB) -o $(TARGET)
.c.o:
	$(CC) -c $(CFLAGS) $<
#dependancy on .h files
main.o: command.h rule.h
