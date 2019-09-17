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
#include "rtimer.h"

#define BUFFER_SIZE                                 128 // Define the payload size here


static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t Buffer[BUFFER_SIZE];
static int len;
static struct timer t;
static rtimer_clock_t rssiTime;
static rtimer_clock_t modemTime;
static rtimer_clock_t packet_arrival_time;
/**
 * Main application entry point.
 */
PROCESS(ping_pong_process, "Ping-pong-LoRa-process");
AUTOSTART_PROCESSES(&ping_pong_process);

PROCESS_THREAD(ping_pong_process, ev, data)
{
    PROCESS_BEGIN();
    rtimer_init();
    timer_set(&t, CLOCK_SECOND*5);
    while (!timer_expired(&t));

    SX1272_tsch_driver.init();


    while(1){
        SX1272_tsch_driver.on();
        //printf("Waiting for packet\n");
        int found = 0;
        while (!SX1272_tsch_driver.pending_packet()){
            if (!found && (SX1272Read(REG_LR_RSSIVALUE) > 70)){
                rssiTime = RTIMER_NOW();
                found = 1;
                printf("rx\n");
            }
            if ((found == 1) && (SX1272Read(REG_LR_MODEMSTAT) & 1)){
                modemTime = RTIMER_NOW();
                found = 2;
            }
            //SX1272_tsch_driver.on();
            //printf("Modemstatus: %d, Rssi: %d\n", SX1272Read(REG_LR_MODEMSTAT), SX1272Read(REG_LR_RSSIVALUE));
        }
        len = SX1272_tsch_driver.read(Buffer, BufferSize);
        found = 0;

        SX1272_tsch_driver.get_object(RADIO_PARAM_LAST_PACKET_TIMESTAMP, &packet_arrival_time, sizeof(packet_arrival_time));

        printf("Received %d bytes, expected time on air: %lu, rssiToModemTime: %lu, rssiToRxIrqTime: %d\n", len, SX1272GetTimeOnAir(MODEM_LORA, len), RTIMERTICKS_TO_US_64(modemTime - rssiTime), (int) RTIMERTICKS_TO_US_64(packet_arrival_time - rssiTime));

        //rtimer_clock_t deadline = RTIMER_NOW() + RTIMER_SECOND/2;
        //while (RTIMER_CLOCK_LT(RTIMER_NOW(), deadline));
        SX1272_tsch_driver.off();
        clock_wait(CLOCK_SECOND/2);
/*
        for (int i = 0; i < len; i++){
            printf("_%x", Buffer[i] & 0xff);
        }
         printf("\n");
*/   }
    
    PROCESS_END();
}
