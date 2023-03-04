#Makefile
OBJ=sam.o iso14443.o ticket.o main.o
#CC =arm-poky-linux-gnueabi-gcc

appnfc:$(OBJ)
	$(CC) -o $@ $^
main.o:main.cpp ticket.h sam.h iso14443.h
	$(CC) -c $<
ticket.o:ticket.cpp ticket.h sam.h iso14443.h
	$(CC) -c $<
sam.o:sam.cpp sam.h
	$(CC) -c $<
iso14443.o:iso14443.cpp iso14443.h
	$(CC) -c $<
.PHONY:clean
clean:
	rm -f $(OBJ) appnfc
