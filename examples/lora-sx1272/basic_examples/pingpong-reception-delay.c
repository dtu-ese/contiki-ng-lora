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
#include <stdint.h>
#include "project-conf.h"
#include "contiki.h"
#include "sx1272-project-conf-macros.h"
#include "sx1272.h"
#include "sx1272-spi-interface.h"/*This is only to read the modem status register to find delays in syncing. Do not include otherwise.*/
#include "sx1272-regs-lora.h"/*This follows same argument as inclusion of spi interface*/
#include "dev/radio.h"
#include "timer.h"
#include "rtimer.h"
#include "lib/random.h"

#define BUFFER_SIZE                                 1 // Define the payload size here

#define SANITY_VALUE 0xaf
#define INTERVAL 250000

static uint8_t transmit_flag = 0;
static uint8_t prev_transmit_flag = 0;
static uint8_t magic_number = SANITY_VALUE;/*Just a value that is always transmitted*/
static uint8_t BufferSize = BUFFER_SIZE;
static uint8_t Buffer[BUFFER_SIZE];
static int len;
static rtimer_clock_t startwait;
static rtimer_clock_t modemTime;
static rtimer_clock_t packet_arrival_time;
static rtimer_clock_t packet_transmit_time;
static rtimer_clock_t target_time;
/**
 * Main application entry point.
 */
static uint8_t rand;
PROCESS(pingpong, "pingpong-reception-delay");
AUTOSTART_PROCESSES(&pingpong);

PROCESS_THREAD(pingpong, ev, data)
{
    PROCESS_BEGIN();
    Buffer[0] = magic_number;
    NETSTACK_RADIO.init();


    while(1){
        
        if (transmit_flag){
            NETSTACK_RADIO.get_object(RADIO_PARAM_LAST_PACKET_TIMESTAMP, &packet_arrival_time, sizeof(packet_arrival_time));
            target_time = packet_arrival_time + US_TO_RTIMERTICKS(INTERVAL) - RADIO_DELAY_BEFORE_TX_SX1272;/*We time our message back*/
            Buffer[0] = magic_number + 1;            
        } else {
            target_time = RTIMER_NOW() + US_TO_RTIMERTICKS(1000);/*We just fire*/
            Buffer[0] = magic_number;            
        }
        NETSTACK_RADIO.prepare(Buffer, BufferSize);
        while(RTIMER_CLOCK_LT(RTIMER_NOW(), target_time));/*We wait a bit, as is done in TSCH source code*/
        packet_transmit_time = RTIMER_NOW();
        NETSTACK_RADIO.transmit(BufferSize);

        NETSTACK_RADIO.on();
        rand = random_rand() & 0xFFFF;/*Random contribution, to ensure they will eventually meet*/


        startwait = RTIMER_NOW();
        RTIMER_BUSYWAIT_UNTIL_ABS(NETSTACK_RADIO.receiving_packet(), startwait, US_TO_RTIMERTICKS(150000) + US_TO_RTIMERTICKS(rand));
        if (NETSTACK_RADIO.receiving_packet()){
            printf("Receiving\n");
            modemTime = RTIMER_NOW();
            while(RTIMER_CLOCK_LT(RTIMER_NOW(), modemTime + TSCH_CONF_PACKET_DURATION(BUFFER_SIZE + 100))){
                if (NETSTACK_RADIO.pending_packet()){
                    printf("Pending\n");
                    break;
                }
                if (!NETSTACK_RADIO.receiving_packet()){
                    printf("Receiving stopped\n");
                }
            }
            if (NETSTACK_RADIO.pending_packet()){
                if ((len = NETSTACK_RADIO.read(Buffer, BUFFER_SIZE) == BUFFER_SIZE) && ((Buffer[0] == magic_number) ||(Buffer[0] == magic_number + 1))){
                    prev_transmit_flag = transmit_flag;
                    transmit_flag = 1;
                }
                printf("buf: %x\n", Buffer[0]);
            }
        } else {
            prev_transmit_flag = transmit_flag;
            transmit_flag = 0;
        }
        NETSTACK_RADIO.off();

        if (prev_transmit_flag && (Buffer[0] == magic_number + 1)){
            NETSTACK_RADIO.get_object(RADIO_PARAM_LAST_PACKET_TIMESTAMP, &packet_arrival_time, sizeof(packet_arrival_time));

            printf("Received %d bytes, expected time on air: %lu us\nInterval between transmission: %d\nactual difference: %lu\nrxdone-modemregister: %lu\n", 
            len, 
            RTIMERTICKS_TO_US_64(TSCH_CONF_PACKET_DURATION(len)), 
            250000, 
            RTIMERTICKS_TO_US_64(packet_arrival_time - packet_transmit_time),
            RTIMERTICKS_TO_US_64(modemTime - packet_arrival_time));
        }

    }
    
    PROCESS_END();
}



/*
            NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &rssi);
            if (!found && (rssi > -90)){
                rssiTime = RTIMER_NOW();
                found = 1;
                printf("rssiHigh\n");
            }
*/
