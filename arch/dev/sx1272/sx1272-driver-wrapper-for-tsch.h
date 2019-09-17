#ifndef SX1272_DRIVER_WRAPPER_FOR_TSCH
#define SX1272_DRIVER_WRAPPER_FOR_TSCH

#include "contiki.h"
#include "dev/radio.h"
#include "sx1272.h"

/*Defines for LoRa*/
#define RF_FREQUENCY                                868300000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define CHANNEL1 868100000 // Hz
#define CHANNEL2 868300000 // Hz
#define CHANNEL3 868500000 // Hz
#define CHANNEL4 867100000 // Hz
#define CHANNEL5 867500000 // Hz
#define CHANNEL6 867700000 // Hz
#define CHANNEL7 867900000 // Hz

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        6         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


extern const struct radio_driver SX1272_tsch_driver;
#define RADIO_DELAY_BEFORE_TX_SX1272 US_TO_RTIMERTICKS(71)//Did not find this in the documentation. Note, is always in RX mode when on. Assumes same flip for TX
#define RADIO_DELAY_BEFORE_RX_SX1272 US_TO_RTIMERTICKS(71)//Table 22, datasheet.
#define TSCH_PACKET_DURATION_SX1272(len) US_TO_RTIMERTICKS(SX1272GetTimeOnAir(MODEM_LORA, len))

#define LORA_BANDWIDTH_HZ 125000
#define SYMBOL_TIME_SX1272  ((2 << LORA_SPREADING_FACTOR)/LORA_BANDWIDTH_HZ*1000)

/*The 8 + 4.25 being preamble time*/
#define RADIO_DELAY_BEFORE_DETECT_SX1272 US_TO_RTIMERTICKS(500)//We only detect after a valid header has arrived + an interrupt is fired.

#endif