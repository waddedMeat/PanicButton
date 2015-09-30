EXECUTABLE=pButton
CC=g++
CFLAGS=-g -Wall
LFLAGS=-lusb-1.0

all: clean
	$(CC) $(CFLAGS) PanicButton.cpp $(LFLAGS) -o $(EXECUTABLE)

install:
	cp $(EXECUTABLE) /usr/local/bin/$(EXECUTABLE)
	cp ./99_panic.rules /etc/udev/rules.d/99_panic.rules
	udevadm control -R
	
clean:
	rm -f $(EXECUTABLE) 

