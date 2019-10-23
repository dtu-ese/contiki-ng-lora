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
#define  SX1272_SPI_SCK             EXT_FLASH_SPI_PIN_SCK
#define  SX1272_SPI_MISO            EXT_FLASH_SPI_PIN_MISO
#define  SX1272_SPI_MOSI            EXT_FLASH_SPI_PIN_MOSI
#define  SX1272_SPI_CS              EXT_FLASH_SPI_PIN_CS
#define  SX1272_SPI_BITRATE         4000000/*sx1272 can do 10 mhz sck according to datasheet. It is unclear if this bitrate is the sck frequency, as spi bitrate depends on setup. Semtech driver does 8 Mhz.*/
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

#ifdef SX1272_CONF_RXDONE_DELAY_USEC
#define SX1272_RXDONE_DELAY_USEC SX1272_CONF_RXDONE_DELAY_USEC
#else
#define SX1272_RXDONE_DELAY_USEC 780
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




/*The 8 + 4.25 being preamble time*/
#endif