OBJS = main.o sgp4ext.o sgp4unit.o sgp4io.o sgp4coord.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

SGP4program:	$(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o SGP4program

main.o:	main.cpp sgp4ext.h sgp4unit.h sgp4io.h sgp4coord.h
	$(CC) $(CFLAGS) main.cpp

sgp4ext.o:	sgp4ext.h sgp4ext.cpp
	$(CC) $(CFLAGS) sgp4ext.cpp

sgp4unit.o:	sgp4unit.h sgp4unit.cpp
	$(CC) $(CFLAGS) sgp4unit.cpp

sgp4io.o:	sgp4io.h sgp4io.cpp
	$(CC) $(CFLAGS) sgp4io.cpp

sgp4coord.o:	sgp4coord.h sgp4coord.cpp
	$(CC) $(CFLAGS) sgp4coord.cpp
