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
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include "RemoteSwitch.cpp"
#include "NewRemoteTransmitter.cpp"
#include "433send.h"

using namespace std;

void usage() {
    std::cerr << "Syntax: 433send $GPIO_PIN_OUT $protocol $address $unit on/off" << std::endl;
    std::cerr << "PIN_OUT using wiringPi pin numbering scheme (15 = TxD / BCM GPIO 14, see https://projects.drogon.net/raspberry-pi/wiringpi/pins/)";
    std::cerr << "Protocol: kaku (oldkaku/newkaku), elro/action" << std::endl;
    std::cerr << "example: 433send 8 oldkaku 2 on" << std::endl;
    std::cerr << "example: 433send 8 newkaku 123 10 dim 5" << std::endl;
    exit(3);
}

int send433mhz(const unsigned short PIN_OUT, deviceType devicetype, int address, int device, deviceCommand command, int value) {
    switch (devicetype) {
        case newkaku:
        {
            NewRemoteTransmitter transmitter(address, PIN_OUT, 260, 3);
            if (device == 0) {
                transmitter.sendGroup(command == on);
            } else {
                if (command != dim) {
                    printf ("device=newkaku address1=%d address2=%d command=%s\n", address, device, command==on?"on":"off");
                    transmitter.sendUnit(device, command == on);
                } else {
                    printf ("device=newkaku address1=%d address2=%d command=dim value=\n", address, device, value);
                    transmitter.sendDim(device, value);
                }
            }
        }
        break;
        case oldkaku:
        {
            printf ("device=oldkaku address1=%c address2=%d command=%s\n", address, device, command==on?"on":"off");
            KaKuSwitch kaKuSwitch(PIN_OUT);
            kaKuSwitch.sendSignal(address, device, command == on);
        }
        break;
        case action:
        {
            printf ("device=action address1=%d address2=%d command=%s\n", address, device, command==on?"on":"off");
            ActionSwitch actionSwitch(PIN_OUT);
            actionSwitch.sendSignal(address, device, command == on);
        }
        break;
        case elro:
        {
            printf ("device=elro address1=%d address2=%d command=%s\n", address, device, command==on?"on":"off");
            ElroSwitch elroSwitch(PIN_OUT);
            elroSwitch.sendSignal(address, device, command == on);
        }
        break;
    }
}




int main(int argc, char **argv) {
        mqd_t mqd;
        struct mq_attr attr;
        int ret;
        char msg[MQ_MESSAGE_MAX_LENGTH];
        ssize_t msg_len;

        // load wiringPi
        if(wiringPiSetup() == -1) {
                printf("WiringPi setup failed. Maybe you haven't installed it yet? (apt-get install wiringpi)");
                exit(1);
        }


        if (argc != 2) {
            printf("Syntax: 433mhzdaemon PIN_OUT");
            exit(1);
        }
        const unsigned short PIN_OUT = (unsigned short) atoi(argv[1]);

        // setup pin and make it low (otherwise transmitter will block other 433 mhz transmitters like remotes)
        pinMode(PIN_OUT, OUTPUT);
        digitalWrite(PIN_OUT, LOW);

    if (argc < 1) usage();
    string protocol = argv[1];


    if( argc < 6 ) { // not enough arguments
        usage();
        exit(1);
    }

    if (protocol.find("kaku") != std::string::npos) {
        if (atol(argv[2])) {
            protocol = "newkaku";
        } else {
            protocol = "oldkaku";
        }
    } else if (protocol.find("newkaku") != std::string::npos) {
        msg[0] = (char) newkaku;
        msg[1] = (char) atol(argv[3]);
        msg[2] = (char) atol(argv[4]);
    } else if (protocol.find("oldkaku") != std::string::npos) {
        msg[0] = (char) oldkaku;
        msg[1] = (char) (argv[3])[0];
        msg[2] = (char) atol(argv[4]);
    } else if (protocol.find("action") != std::string::npos) {
        // msg[0] = devicetype
        msg[0] = (char) action;
        msg[1] = atol(argv[3]);
        msg[2] = *argv[4];
    } else if (protocol.find("elro") != std::string::npos) {
        // msg[0] = devicetype
        msg[0] = (char) elro;
        msg[1] = atol(argv[3]);
        msg[2] = *argv[4];
    }

    string statestr = argv[5];
    msg[3] = 0;

    if( statestr.compare("on") == 0 ) {
        // msg[3] = state
        msg[3]  = (char) on;
    } else if( statestr.compare("off") == 0 ) {
        msg[3] = (char) off;
    } else if( statestr.compare("dim") == 0 ) {
        msg[3] = (char) dim;
       // msg[4] = value
        msg[4] = (char) atol(argv[6]);
    } else {
        printf ("Only on, off, or dim state are valid");
        exit(3);
    }

    if (msg_len > 4) {
        send433mhz(PIN_OUT, (deviceType) msg[0], msg[1], msg[2], (deviceCommand)  msg[3], msg[4]);
    } else {
        std::err << "msg_len too short: " << msg_len << std::endl;
    }


    return 0;
}

