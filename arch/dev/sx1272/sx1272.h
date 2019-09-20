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

/*We do this before including contiki.h, to ensure that the SX1272 Radio is used instead of the radio the device is born with*/
#ifdef  USE_SX1272_AS_STANDARD_RADIO
#define NETSTACK_CONF_RADIO SX1272_radio_driver
#endif

#include "contiki.h"
#include "dev/radio.h"
#include "dev/gpio-hal.h"
#include "dev/spi.h"


/*Defines for SX1272 user, to be done in config.h. If not done, the following will be defaulted to:*/
#ifdef  SX1272_CONF_TX_OUTPUT_POWER
#define SX1272_TX_OUTPUT_POWER        SX1272_CONF_TX_OUTPUT_POWER
#else
#define TX_OUTPUT_POWER                             14        // dBm
#endif

#ifdef  SX1272_CONF_SPREADING_FACTOR
#define SX1272_SPREADING_FACTOR        SX1272_CONF_SPREADING_FACTOR
#else
#define SX1272_SPREADING_FACTOR                       7         // [SF7..SF12]
#endif

#ifdef  SX1272_CONF_CODINGRATE
#define SX1272_CODINGRATE        SX1272_CONF_CODINGRATE
#else
#define SX1272_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#endif

#ifdef  SX1272_CONF_PREAMBLE_LENGTH
#define SX1272_PREAMBLE_LENGTH        SX1272_CONF_PREAMBLE_LENGTH
#else
#define SX1272_PREAMBLE_LENGTH                        6         // Same for Tx and Rx
#endif

#ifdef  SX1272_CONF_TX_SYMBOL_TIMEOUT
#define SX1272_SYMBOL_TIMEOUT        SX1272_CONF_TX_SYMBOL_TIMEOUT
#else
#define SX1272_SYMBOL_TIMEOUT                         5         // Symbols
#endif

#ifdef  SX1272_CONF_BANDWIDTH
#define SX1272_BANDWIDTH        SX1272_CONF_BANDWIDTH
#else
#define SX1272_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#endif

#ifdef  SX1272_CONF_SPI_CONTROLLER
#define SX1272_SPI_CONTROLLER        SX1272_CONF_SPI_CONTROLLER
#else
#define SX1272_SPI_CONTROLLER                              0
#endif

#ifdef   SX1272_CONF_PINS
#define  SX1272_RESET_PIN           SX1272_CONF_RESET_PIN
#define  SX1272_ANT_SWITCH          SX1272_CONF_ANT_SWITCH
#define  SX1272_SPI_SCK             SX1272_CONF_SPI_SCK
#define  SX1272_SPI_MISO            SX1272_CONF_SPI_MISO
#define  SX1272_SPI_MOSI            SX1272_CONF_SPI_MOSI
#define  SX1272_SPI_CS              SX1272_CONF_SPI_CS
#define  SX1272_SPI_BITRATE         SX1272_CONF_SPI_BITRATE
#else
/*We default to just using the pins meant for the external flash*/
#define  SX1272_RESET_PIN           BOARD_IOID_DIO15
#define  SX1272_ANT_SWITCH          BOARD_IOID_DIO30
#define  SX1272_SPI_SCK             EXT_FLASH_SPI_PIN_SCK
#define  SX1272_SPI_MISO            EXT_FLASH_SPI_PIN_MISO
#define  SX1272_SPI_MOSI            EXT_FLASH_SPI_PIN_MOSI
#define  SX1272_SPI_CS              EXT_FLASH_SPI_PIN_CS
#define  SX1272_SPI_BITRATE         1000000
#endif

#define SX1272_SPI_PHASE            0
#define SX1272_SPI_POL              0

  uint8_t spi_pha;                  /* SPI mode phase */
  uint8_t spi_pol;                  /* SPI mode polarity */
  uint8_t spi_controller;           /* ID of SPI controller to use */


/*Will we allow enabling of interrupts? Also note that this should then exclude poll-mode for now*/
#ifdef   SX1272_INTERRUPT_DRIVER
#define  SX1272_DIO0 SX1272_CONF_DIO0
#define  SX1272_DIO1 SX1272_CONF_DIO1
#define  SX1272_DIO2 SX1272_CONF_DIO2
#define  SX1272_DIO3 SX1272_CONF_DIO3
#ifdef   GPIO_HAL_PORT_PIN_NUMBERING
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

#ifdef   GPIO_HAL_PORT_PIN_NUMBERING
#define  SX1272_SPI_SCK_PORT             SX1272_CONF_SPI_SCK_PORT
#define  SX1272_SPI_MISO_PORT            SX1272_CONF_SPI_MISO_PORT
#define  SX1272_SPI_MOSI_PORT            SX1272_CONF_SPI_MOSI_PORT
#define  SX1272_SPI_CS_PORT              SX1272_CONF_SPI_CS_PORT
#define  SX1272_RESET_PIN_PORT           SX1272_CONF_RESET_PIN_PORT
#define  SX1272_ANT_SWITCH_PIN_PORT      SX1272_CONF_ANT_SWITCH_PIN_PORT

#else/*If there is no port-pin numbering*/
#define  SX1272_RESET_PIN_PORT           GPIO_HAL_NULL_PORT
#define  SX1272_ANT_SWITCH_PIN_PORT      GPIO_HAL_NULL_PORT
#endif

/*Defines for LoRa*/
#define LORA_CHANNELS = {\
    868100000, \
    868300000, \
    868500000, \
    867100000, \
    867300000, \
    867500000, \
    867700000, \
    867900000  \
    }


/*Should everything below this line be defined somewhere else?*/
uint32_t Lora_Channels[8];
uint8_t current_channel;

typedef struct SX1272pins_t {
    gpio_hal_port_t       reset_port;
    gpio_hal_port_t       ant_switch_port;
    gpio_hal_pin_t        reset;
    gpio_hal_pin_t        ant_switch;
    spi_device_t          spi;
} SX1272pins_t;

extern const struct radio_driver SX1272_radio_driver;

#define RADIO_DELAY_BEFORE_TX_SX1272 US_TO_RTIMERTICKS(71)//Did not find this in the documentation. Note, is always in RX mode when on. Assumes same flip for TX
#define RADIO_DELAY_BEFORE_RX_SX1272 US_TO_RTIMERTICKS(71)//Table 22, datasheet.
#define TSCH_PACKET_DURATION_SX1272(len) US_TO_RTIMERTICKS(SX1272GetTimeOnAir(MODEM_LORA, len))

#define SYMBOL_TIME_SX1272  ((2 << SX1272_SPREADING_FACTOR)/SX1272_BANDWIDTH_HZ*1000)
/*The 8 + 4.25 being preamble time*/

#endif