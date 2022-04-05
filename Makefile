#Do not edit the contents of this file.
CC = gcc
CFLAGS = -Wall -g -std=gnu99
LDFLAGS = -lrt -lpthread
TARGET = Question1  
OBJFILES = Question1.o
all: $(TARGET)

Question1: bankersAlgo.c
	$(CC) $(CFLAGS) -o bankersAlgo bankersAlgo.c $(LDFLAGS)
	
runq1: bankersAlgo
	./bankersAlgo 10 5 7 8
	
clean:
	rm -f $(OBJFILES) $(TARGET) *~ 