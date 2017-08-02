all: 433mhzdaemon 433mhztool

433mhzdaemon: 433mhzdaemon.cpp
	g++ -o 433mhzdaemon 433mhzdaemon.cpp -I/usr/local/include  -L/usr/local/lib -lwiringPi -lrt

433mhztool: 433mhztool.cpp
	g++ -o 433mhztool 433mhztool.cpp -lrt

install: 433mhzdaemon 433mhztool
	cp 433mhzdaemon 433mhztool /usr/bin/
