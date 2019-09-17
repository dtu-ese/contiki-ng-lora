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
#include "contiki.h"
#include "sx1272-driver-wrapper-for-tsch.h"
#include "timer.h"

#define BUFFER_SIZE                                 64 // Define the payload size here

static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t Buffer[BUFFER_SIZE];
static struct timer t;
static int j;

/**
 * Main application entry point.
 */
PROCESS(ping_pong_process, "Ping-pong-LoRa-process");
AUTOSTART_PROCESSES(&ping_pong_process);

PROCESS_THREAD(ping_pong_process, ev, data)
{
    PROCESS_BEGIN();
    timer_set(&t, CLOCK_SECOND*5);
    while (!timer_expired(&t));

    SX1272_tsch_driver.init();

    Buffer[0] = 'A';
    Buffer[1] = 'B';
    Buffer[2] = 'C';
    Buffer[3] = 'D';
    // We fill the buffer with numbers for the payload 
    static int i;
    for( i = 4; i < BufferSize; i++ )
    {
        Buffer[i] = i - 4;
    }
    j = 0;

        SX1272_tsch_driver.prepare(Buffer, 1);

        SX1272_tsch_driver.transmit(1);

    timer_set(&t, 2);
        while(!timer_expired(&t));

    while( 1 )
    {
        SX1272_tsch_driver.prepare(Buffer, 1);
        printf("Transmitting\n");
        SX1272_tsch_driver.transmit(1);
        timer_reset(&t);
        while(!timer_expired(&t));
    }
    PROCESS_END();
}
