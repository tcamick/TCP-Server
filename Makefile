CFLAGS = -g -Wall
CC = gcc
JCC = javac

all: server c_client TCPclient.class

objects1 = TCPserverMain.o TCPserver.o

objects2 = TCPmain.o TCPclient.o

objects3 = TCPclient.java

server: $(objects1)
	$(CC) -o server $(objects1) -lpthread
	
c_client: $(objects2)
	$(CC) -o c_client $(objects2)

TCPclient.class: $(objects3)
	$(JCC) $(objects3)

TCPserver.o: TCPserver.c
TCPserverMain.o: TCPserverMain.c

TCPclient.o: TCPclient.c
TCPmain.o: TCPmain.c


.PHONY : clean
clean: 
	rm server c_client *.class $(objects1) $(objects2)
