all: 433send

433send: 433send.cpp
	g++ -o 433send 433send.cpp -I/usr/local/include  -L/usr/local/lib -lwiringPi -lrt

install: 433send
	cp 433send /usr/bin/
