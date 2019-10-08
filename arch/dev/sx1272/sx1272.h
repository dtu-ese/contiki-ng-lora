/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef SX1272_H_
#define SX1272_H_
/*This is the main file, that should be included if a LoRa project using the SX1272 is being designed*/

/*Defines for SX1272 user, to be done in project-conf.h. If not done, the following will be defaulted to:*/
#ifdef  SX1272_CONF_TX_OUTPUT_POWER
#define SX1272_TX_OUTPUT_POWER        SX1272_CONF_TX_OUTPUT_POWER
#else
#define SX1272_TX_OUTPUT_POWER                             14        // dBm
#endif

#ifdef  SX1272_CONF_TX_SYMBOL_TIMEOUT
#define SX1272_SYMBOL_TIMEOUT        SX1272_CONF_TX_SYMBOL_TIMEOUT
#else
#define SX1272_SYMBOL_TIMEOUT                         5         // Symbols
#endif



#ifdef  SX1272_CONF_SPI_CONTROLLER
#define SX1272_SPI_CONTROLLER        SX1272_CONF_SPI_CONTROLLER
#else
#define SX1272_SPI_CONTROLLER                              0
#endif

#ifdef   SX1272_CONF_PINS
#define  SX1272_RESET_PIN           SX1272_CONF_RESET_PIN
//#define  SX1272_ANT_SWITCH          SX1272_CONF_ANT_SWITCH
#define  SX1272_SPI_SCK             SX1272_CONF_SPI_SCK
#define  SX1272_SPI_MISO            SX1272_CONF_SPI_MISO
#define  SX1272_SPI_MOSI            SX1272_CONF_SPI_MOSI
#define  SX1272_SPI_CS              SX1272_CONF_SPI_CS
#define  SX1272_SPI_BITRATE         SX1272_CONF_SPI_BITRATE
#else
/*We default to just using the pins meant for the external flash*/
#define  SX1272_RESET_PIN           BOARD_IOID_DIO15
//#define  SX1272_ANT_SWITCH          BOARD_IOID_DIO30
#define  SX1272_SPI_SCK             EXT_FLASH_SPI_PIN_SCK
#define  SX1272_SPI_MISO            EXT_FLASH_SPI_PIN_MISO
#define  SX1272_SPI_MOSI            EXT_FLASH_SPI_PIN_MOSI
#define  SX1272_SPI_CS              EXT_FLASH_SPI_PIN_CS
#define  SX1272_SPI_BITRATE         1000000
#endif

#define SX1272_SPI_PHASE            0
#define SX1272_SPI_POL              0

/*Will we allow enabling of interrupts? Also note that this should then exclude poll-mode for now*/
#ifdef   SX1272_INTERRUPT_DRIVER
#define  SX1272_DIO0 SX1272_CONF_DIO0
#define  SX1272_DIO1 SX1272_CONF_DIO1
#define  SX1272_DIO2 SX1272_CONF_DIO2
#define  SX1272_DIO3 SX1272_CONF_DIO3
#if      GPIO_HAL_PORT_PIN_NUMBERING
#define  SX1272_DIO0_PORT SX1272_CONF_DIO0_PORT
#define  SX1272_DIO1_PORT SX1272_CONF_DIO1_PORT
#define  SX1272_DIO2_PORT SX1272_CONF_DIO2_PORT
#define  SX1272_DIO3_PORT SX1272_CONF_DIO3_PORT
#else
#define  SX1272_DIO0_PORT GPIO_HAL_NULL_PORT
#define  SX1272_DIO1_PORT GPIO_HAL_NULL_PORT
#define  SX1272_DIO2_PORT GPIO_HAL_NULL_PORT
#define  SX1272_DIO3_PORT GPIO_HAL_NULL_PORT
#endif
#endif

#if      GPIO_HAL_PORT_PIN_NUMBERING
#define  SX1272_SPI_SCK_PORT             SX1272_CONF_SPI_SCK_PORT
#define  SX1272_SPI_MISO_PORT            SX1272_CONF_SPI_MISO_PORT
#define  SX1272_SPI_MOSI_PORT            SX1272_CONF_SPI_MOSI_PORT
#define  SX1272_SPI_CS_PORT              SX1272_CONF_SPI_CS_PORT
#define  SX1272_RESET_PIN_PORT           SX1272_CONF_RESET_PIN_PORT
//#define  SX1272_ANT_SWITCH_PIN_PORT      SX1272_CONF_ANT_SWITCH_PIN_PORT

#else/*If there is no port-pin numbering*/
#define  SX1272_RESET_PIN_PORT           GPIO_HAL_NULL_PORT
//#define  SX1272_ANT_SWITCH_PIN_PORT      GPIO_HAL_NULL_PORT
#endif

/*TSCH Timeslot Build - Crafted for SF7 as default, must be handcrafted otherwise*/
#ifdef TSCH_SX1272_TIMESLOT_CONF_CCA_OFFSET
#define TSCH_SX1272_TIMESLOT_CCA_OFFSET TSCH_SX1272_TIMESLOT_CONF_CCA_OFFSET
#else
#define TSCH_SX1272_TIMESLOT_CCA_OFFSET 180
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_CCA
#define TSCH_SX1272_TIMESLOT_CCA_OFFSET TSCH_SX1272_TIMESLOT_CONF_CCA
#else
#define TSCH_SX1272_TIMESLOT_CCA 128
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_TX_OFFSET
#define TSCH_SX1272_TIMESLOT_TX_OFFSET TSCH_SX1272_TIMESLOT_CONF_TX_OFFSET
#else
#define TSCH_SX1272_TIMESLOT_TX_OFFSET 9000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_RX_OFFSET
#define TSCH_SX1272_TIMESLOT_RX_OFFSET TSCH_SX1272_TIMESLOT_CONF_RX_OFFSET
#else
#define TSCH_SX1272_TIMESLOT_RX_OFFSET 7000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_RX_ACK_DELAY
#define TSCH_SX1272_TIMESLOT_RX_ACK_DELAY TSCH_SX1272_TIMESLOT_CONF_RX_ACK_DELAY
#else
#define TSCH_SX1272_TIMESLOT_RX_ACK_DELAY 5000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_TX_ACK_DELAY
#define TSCH_SX1272_TIMESLOT_TX_ACK_DELAY TSCH_SX1272_TIMESLOT_CONF_TX_ACK_DELAY
#else
#define TSCH_SX1272_TIMESLOT_TX_ACK_DELAY 6000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_RXWAIT
#define TSCH_SX1272_TIMESLOT_RXWAIT TSCH_SX1272_TIMESLOT_CONF_RXWAIT
#else
#define TSCH_SX1272_TIMESLOT_RXWAIT 8000
#endif
#ifdef TSCH_SX1272_TIMESLOT_CONF_ACKWAIT
#define TSCH_SX1272_TIMESLOT_ACKWAIT TSCH_SX1272_TIMESLOT_CONF_ACKWAIT
#else
#define TSCH_SX1272_TIMESLOT_ACKWAIT 6000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_RXTX
#define TSCH_SX1272_TIMESLOT_RXTX TSCH_SX1272_TIMESLOT_CONF_RXTX
#else
#define TSCH_SX1272_TIMESLOT_RXTX 192
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_MAXACK
#define TSCH_SX1272_TIMESLOT_MAXACK TSCH_SX1272_TIMESLOT_CONF_MAXACK
#else
#define TSCH_SX1272_TIMESLOT_MAXACK 48000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_MAXTX
#define TSCH_SX1272_TIMESLOT_MAXTX TSCH_SX1272_TIMESLOT_CONF_MAXTX
#else
#define TSCH_SX1272_TIMESLOT_MAXTX 209000
#endif

#ifdef TSCH_SX1272_TIMESLOT_CONF_TIMESLOTLENGTH
#define TSCH_SX1272_TIMESLOT_TIMESLOTLENGTH TSCH_SX1272_TIMESLOT_CONF_TIMESLOTLENGTH
#else
#define TSCH_SX1272_TIMESLOT_TIMESLOTLENGTH 300000
#endif


/*Defines for LoRa*/
#define LORA_CHANNELS {\
    868100000, \
    868300000, \
    868500000, \
    867100000, \
    867300000, \
    867500000, \
    867700000, \
    867900000  \
    }

/*We load contiki.h now to ensure that the type radio_driver is known...*/
#include "contiki.h"
#include "net/mac/tsch/tsch.h"
#include "dev/radio.h"
/*Should everything below this line be defined somewhere else?*/
uint32_t Lora_Channels[8];
uint8_t current_channel;


extern const struct radio_driver sx1272_radio_driver;
extern const tsch_timeslot_timing_usec tsch_timeslot_timing_sx1272;

#define USEC_SLEEP_TO_STANDBY 250
#define USEC_FREQ_SYNTH 60
#define USEC_STANDBY_TO_RX 71 
#define USEC_STANDBY_TO_TX 120
#define USEC_CHANNEL_HOP   20

/*Maybe we should note that the LoRa preambles are so slow that this doesn't matter much*/
#define RADIO_DELAY_BEFORE_TX_SX1272 US_TO_RTIMERTICKS(USEC_STANDBY_TO_TX + USEC_FREQ_SYNTH)//Did not find this in the documentation. Note, is always in RX mode when on. Assumes same flip for TX
#define RADIO_DELAY_BEFORE_RX_SX1272 US_TO_RTIMERTICKS(USEC_STANDBY_TO_RX + USEC_FREQ_SYNTH)//Table 22, datasheet.



/*The 8 + 4.25 being preamble time*/
#endif