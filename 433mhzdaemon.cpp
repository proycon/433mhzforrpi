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
#include <mqueue.h>
#include "RemoteSwitch.cpp"
#include "NewRemoteTransmitter.cpp"
#include "433mhzdaemon.h"

using namespace std;

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
            printf("PIN_OUT using wiringPi pin numbering scheme (15 = TxD / BCM GPIO 14, see https://projects.drogon.net/raspberry-pi/wiringpi/pins/)");
            exit(1);
        }
        const unsigned short PIN_OUT = (unsigned short) atoi(argv[1]);

        // setup pin and make it low (otherwise transmitter will block other 433 mhz transmitters like remotes)
        pinMode(PIN_OUT, OUTPUT);
        digitalWrite(PIN_OUT, LOW);

         /* Form the queue attributes */
        attr.mq_flags = 0; /* i.e mq_send will be block if message queue is full */
        attr.mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES;
        attr.mq_msgsize = MQ_MESSAGE_MAX_LENGTH;
        attr.mq_curmsgs = 0; /* mq_curmsgs is dont care */

        /* Create message queue */
        mqd = mq_open(MQ_NAME, O_RDONLY | O_CREAT, MQ_MODE, &attr);
        if( mqd != (mqd_t)-1 ) {
                printf(" Message Queue Opened\n");

                printf(" Receiving message .... \n");
                while (1)
                {
                    msg_len = mq_receive(mqd, msg, MQ_MESSAGE_MAX_LENGTH, NULL);
                    if(msg_len < 0)
                    {
                        printf(" Failed");
//                        break;
                    }
                    else
                    {
                        msg[MQ_MESSAGE_MAX_LENGTH-1] = '\0';
                        printf(" Successfully received %d bytes, ", (int)msg_len);
                        if (msg_len > 4) send433mhz(PIN_OUT, (deviceType) msg[0], msg[1], msg[2], (deviceCommand)  msg[3], msg[4]);
                    }
                }

                /* Close the message queue */
                ret = mq_close(mqd);
                if(ret)
                        perror(" Message queue close failed");
                else
                        printf(" Message Queue Closed\n");

                ret = mq_unlink(MQ_NAME);
                if(ret)
                        perror(" Message queue unlink failed");
                else
                        printf(" Message Queue unlinked\n");
        } else {
                perror(" Message queue open failed ");
        }

        return 0;
}

