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
#include "contiki.h"
#include "sx1272.h"
#include "timer.h"
#include "sx1272-spi-interface.h"/*This is only to read the modem status register to find delays in syncing. Do not include otherwise.*/
#include "sx1272-regs-lora.h"/*This follows same argument as inclusion of spi interface*/

#define BUFFER_SIZE                                 64 // Define the payload size here

static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t Buffer[BUFFER_SIZE];
static struct timer t;
static int j;

/**
 * Main application entry point.
 */
PROCESS(lora_sender, "lora-sender-process");
AUTOSTART_PROCESSES(&lora_sender);

PROCESS_THREAD(lora_sender, ev, data)
{
    PROCESS_BEGIN();
    timer_set(&t, CLOCK_SECOND);
    while(!timer_expired(&t));

    NETSTACK_RADIO.init();

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

    timer_set(&t, CLOCK_SECOND);

    while(1)
    {
        NETSTACK_RADIO.prepare(Buffer, BufferSize);

        rtimer_clock_t goalTime = RTIMER_NOW() + US_TO_RTIMERTICKS(5000);

        /*We busy wait for transmission to complete */
        while(RTIMER_CLOCK_LT(RTIMER_NOW(), goalTime));

        printf("T\n");
        NETSTACK_RADIO.transmit(BufferSize);
        printf("opmode: %d, modemconfig1: %d, modemconfig2: %d\n", spi_read_sx1272(REG_LR_OPMODE), spi_read_sx1272(REG_LR_MODEMCONFIG1), spi_read_sx1272(REG_LR_MODEMCONFIG2));

        printf("Done transmitting. Transmitted %d bytes\n", BufferSize);
        printf("Time estimate: %luus\n", RTIMERTICKS_TO_US_64(TSCH_CONF_PACKET_DURATION(BufferSize)));

        for( i = 4; i < BufferSize; i++ )
        {
            Buffer[i] = i + j - 4;
        }
        j = (j + 1) % 4;
        timer_reset(&t);
        while(!timer_expired(&t));
    }
    PROCESS_END();
}
