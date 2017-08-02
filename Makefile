all: 433send

433send: 43send.cpp
	g++ -o 433mhzdaemon 433mhzdaemon.cpp -I/usr/local/include  -L/usr/local/lib -lwiringPi -lrt

install: 433send
	cp 433send /usr/bin/
