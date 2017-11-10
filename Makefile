CC = g++ $(CFLAGS)
CFLAGS = -g -O0 -Wall

all: wordfreq phoneBook

wordfreq: wordfreq.o HashSet.o
	$(CC) -o wordfreq wordfreq.o HashSet.o

wordfreq.o: wordfreq.cpp HashSet.h
	$(CC) -c wordfreq.cpp

phoneBook: phoneBook.o HashSet.o
	$(CC) -o phoneBook phoneBook.o HashSet.o

phoneBook.o: phoneBook.cpp HashSet.h
	$(CC) -c phoneBook.cpp

HashSet.o: HashSet.cpp HashSet.h
	$(CC) -c HashSet.cpp

clean:
	rm -f *.o wordfreq phoneBook *\~ core*
