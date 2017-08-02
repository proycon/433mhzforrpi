#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "433mhzdaemon.h"

using namespace std;

void usage() {
    std::cerr << "Syntax: 433mhztool $protocol $address $unit on/off" << std::endl;
    std::cerr << "Protocol: kaku (oldkaku/newkaku), elro/action" << std::endl;
    std::cerr << "example: 433mhztool oldkaku 2 on" << std::endl;
    std::cerr << "example: 433mhztool newkaku 123 10 dim 5" << std::endl;
    exit(3);
}

int main(int argc, char **argv) {
	mqd_t mqd;
	int ret;

    char msg[MQ_MESSAGE_MAX_LENGTH];
    ssize_t msg_len;


    if (argc < 1) usage();
    string protocol = argv[1];


    if( argc < 5 ) { // not enough arguments
        if (protocol.find("kaku") != std::string::npos) {
        	std::cout << "usage: " << argv[0] << " address device state level" << std::endl;
        	std::cout << "example: " << argv[0] << " for oldkaku : A 2 on" << std::endl;
        	std::cout << "example: " << argv[0] << " for newkaku : 123 10 dim 5" << std::endl;
        } else if ((protocol.find("action") != std::string::npos) || (protocol.find("elro") != std::string::npos)) {
	        std::cout << "usage: " << argv[0] << " dipchannel socket state" << std::endl;
        	std::cout << "example: " << argv[0] << " 18 B on" << std::endl;
        } else {
            printf ("Unknown protocol '%s', use kaku, newkaku, oldkaku, elro or action\n",argv[0]);
            exit(2);
        }
        exit(1);
    }

    if (protocol.find("kaku") != std::string::npos) {
        if (atol(argv[1])) {
            protocol = "newkaku";
        } else {
            protocol = "oldkaku";
        }
    } else if (protocol.find("newkaku") != std::string::npos) {
        msg[0] = (char) newkaku;
        msg[1] = (char) atol(argv[2]);
        msg[2] = (char) atol(argv[3]);
    } else if (protocol.find("oldkaku") != std::string::npos) {
        msg[0] = (char) oldkaku;
        msg[1] = (char) (argv[2])[0];
        msg[2] = (char) atol(argv[3]);
    } else if (protocol.find("action") != std::string::npos) {
        // msg[0] = devicetype
        msg[0] = (char) action;
        msg[1] = atol(argv[2]);
        msg[2] = *argv[3];
    } else if (protocol.find("elro") != std::string::npos) {
        // msg[0] = devicetype
        msg[0] = (char) elro;
        msg[1] = atol(argv[2]);
        msg[2] = *argv[3];
    }

    string statestr = argv[4];
    msg[4] = 0;

    if( statestr.compare("on") == 0 ) {
        // msg[3] = state
        msg[3]  = (char) on;
    } else if( statestr.compare("off") == 0 ) {
        msg[3] = (char) off;
    } else if( statestr.compare("dim") == 0 ) {
        msg[3] = (char) dim;
       // msg[4] = value
        msg[4] = (char) atol(argv[4]);
    } else {
        printf ("Only on, off, or dim state are valid");
        exit(3);
    }

    /* Open the message queue. Message queue is already created from a different process */
    mqd = mq_open(MQ_NAME, O_WRONLY);
    if ( mqd != (mqd_t)-1 ) {
        printf(" Sending message to 433mhzdaemon.... ");
        ret = mq_send(mqd, msg, MQ_MESSAGE_MAX_LENGTH, MQ_MESSAGE_PRIORITY);
        if (ret) perror("Failed"); else printf("Done\n");
        /* Close the message queue */
        ret = mq_close(mqd);
        if (ret) perror(" Message queue close failed"); else printf(" Message Queue Closed\n");
    } else {
        perror(" Message queue open failed, is 433mhzdaemon running?");
        exit(4);
    }

	return 0;
}
