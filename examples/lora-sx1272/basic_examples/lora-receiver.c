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
#include "sx1272-spi-interface.h"/*This is only to read the modem status register to find delays in syncing. Do not include otherwise.*/
#include "sx1272-regs-lora.h"/*This follows same argument as inclusion of spi interface*/
#include "dev/radio.h"
#include "timer.h"
#include "rtimer.h"

#define BUFFER_SIZE                                 128 // Define the payload size here


static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t Buffer[BUFFER_SIZE];
static int len;
static rtimer_clock_t rssiTime;
static rtimer_clock_t modemTime;
static rtimer_clock_t packet_arrival_time;
/**
 * Main application entry point.
 */
static int found;
static radio_value_t rssi;
static struct timer t;

PROCESS(lora_receiver, "LoRa Receiver");
AUTOSTART_PROCESSES(&lora_receiver);

PROCESS_THREAD(lora_receiver, ev, data)
{
    PROCESS_BEGIN();
    timer_set(&t, CLOCK_SECOND);
    while(!timer_expired(&t));

    NETSTACK_RADIO.init();

    while(1){
        NETSTACK_RADIO.on();

        found = 0;
        while (!NETSTACK_RADIO.pending_packet()){
            NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &rssi);
            if (!found && (rssi > -90)){
                rssiTime = RTIMER_NOW();
                found = 1;
                printf("rssiHigh\n");
            }
            if ((found == 1) && (spi_read_sx1272(REG_LR_MODEMSTAT) & 1)){
                modemTime = RTIMER_NOW();
                found = 2;
            }
        }
        len = NETSTACK_RADIO.read(Buffer, BufferSize);
        found = 0;

        NETSTACK_RADIO.get_object(RADIO_PARAM_LAST_PACKET_TIMESTAMP, &packet_arrival_time, sizeof(packet_arrival_time));

        printf("Received %d bytes, expected time on air: %lu us\nrssiToModemStatusRegisterTime: %luus\nrssiToRxIrqTime: %luus\n", len, RTIMERTICKS_TO_US_64(TSCH_CONF_PACKET_DURATION(len)), RTIMERTICKS_TO_US_64(modemTime - rssiTime), RTIMERTICKS_TO_US_64(packet_arrival_time - rssiTime));
    }
    
    PROCESS_END();
}
