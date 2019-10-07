/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Ping-Pong implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <string.h>
#include <stdio.h>
#include "project-conf.h"
#include "sx1272.h"
#include "contiki.h"
#include "timer.h"
#include "dev/radio.h"

#define BUFFER_SIZE                                 4
static uint8_t Buffer[BUFFER_SIZE];
static struct timer t;

/**
 * Main application entry point.
 */
PROCESS(jammer, "LoRa-Jammer");
AUTOSTART_PROCESSES(&jammer);

PROCESS_THREAD(jammer, ev, data)
{
    PROCESS_BEGIN();
    timer_set(&t, CLOCK_SECOND*5);
    while (!timer_expired(&t));

    NETSTACK_RADIO.init();

    Buffer[0] = 'A';
    Buffer[1] = 'B';
    Buffer[2] = 'C';
    Buffer[3] = 'D';
    // We fill the buffer with numbers for the payload 

    NETSTACK_RADIO.prepare(Buffer, 4);
    NETSTACK_RADIO.transmit(4);

    timer_set(&t, 2);
    while(!timer_expired(&t));

    while(1)
    {
        NETSTACK_RADIO.prepare(Buffer, 4);
        printf("Transmitting\n");
        NETSTACK_RADIO.transmit(4);
        timer_reset(&t);
        while(!timer_expired(&t));
    }
    PROCESS_END();
}
