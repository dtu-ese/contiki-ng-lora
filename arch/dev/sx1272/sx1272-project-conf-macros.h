#ifndef SX1272_PROJECT_CONF_MACROS
#define SX1272_PROJECT_CONF_MACROS
/*We need these macros defined early, mainly for Energest and if we want to use the SX1272 as the standard MAC Layer Radio*/
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

#ifdef  SX1272_CONF_BANDWIDTH
#define SX1272_BANDWIDTH        SX1272_CONF_BANDWIDTH
#else
#define SX1272_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#endif

/*Energest*/
#define ENERGEST_CONF_PLATFORM_ADDITIONS ENERGEST_TYPE_SX1272_TRANSMIT,ENERGEST_TYPE_SX1272_RX,ENERGEST_TYPE_SX1272_SLEEP,ENERGEST_TYPE_SX1272_STANDBY

/*Other issues with contiki-ng, TSCH and LoRa is timeslot timing. Due to the nature of the way computation from us to rtimer ticks is done,
 *it is potentially necessarry to ensure that the calls to RTIMERTICKS_TO_US always be a call to the 64-bit version. If this is not done, a
 *bad overflow will happen, and the timeslot timings (Potentially along with other timings) Will break.
 *Apart from this, the type of the tsch_timeslot_timing_usec has to be changed from uint16_t to uint32_t, as LoRa will lead to transmissions lengths
 *longer than 2^16-1. This might mean changing a few other values in the system as well.
 *All changes to contiki-ng source necessary for LoRa to work should be well described here.
 *This also means, that in tsch.c, the tsch_timing_us[tsch_ts_elements_count] must be a uint32_t instead of uint16_t.
 *static const uint16_t *tsch_default_timing_us (tsch.c) must be uint32_t.
 */

/*We do this before including contiki.h, to ensure that the SX1272 Radio is used instead of the radio the device is born with*/

#define SYMBOL_TIME_SX1272  ((uint32_t)(((1000 /*From s to us*/)*(1 << SX1272_SPREADING_FACTOR))/((SX1272_BANDWIDTH == 0) ? (125000/1000) : (SX1272_BANDWIDTH == 1) ? (250000/1000) : (500000/1000))))

/*Instead of calling ceil, a +1 and int cast is done. The potential negative part occurs twice, hence the macro*/
#define SX1272_POTENTIAL_NEGATIVE_CONTRIBUTION_TO_TIME(len) ((int)(((8 * len - 4 * SX1272_SPREADING_FACTOR +\
                                28 + 16 - 20)/\
                                ( double )( 4 * (SX1272_SPREADING_FACTOR)))+1)*\
                                ( SX1272_CODINGRATE + 4))
                                
#define SX1272_PAYLOAD_TIME(len) ((uint32_t)(SYMBOL_TIME_SX1272*(8+(\
                                SX1272_POTENTIAL_NEGATIVE_CONTRIBUTION_TO_TIME(len) \
                                > 0 ? \
                                \
                                SX1272_POTENTIAL_NEGATIVE_CONTRIBUTION_TO_TIME(len) \
                                \
                                : 0)))) /*In us*/

#define SX1272_PHY_OVERHEAD        ((uint32_t)(SYMBOL_TIME_SX1272*(4.25+SX1272_PREAMBLE_LENGTH)))/*In us*/
/*We have to override this macro, as the 'per byte' time is dependent on the number of bytes*/
#ifdef  USE_SX1272_AS_STANDARD_RADIO

#define NETSTACK_CONF_RADIO sx1272_radio_driver
#define NETSTACK_RADIO NETSTACK_CONF_RADIO

#define TSCH_CONF_PACKET_DURATION(len) US_TO_RTIMERTICKS(SX1272_PHY_OVERHEAD + SX1272_PAYLOAD_TIME(len))

#define RTIMER_CONF_CLOCK_SIZE 8

#ifndef TSCH_CONF_DEFAULT_TIMESLOT_TIMING
#define TSCH_CONF_DEFAULT_TIMESLOT_TIMING tsch_timeslot_timing_sx1272
#endif /*TSCH_CONF_DEFAULT_TIMESLOT_TIMING*/


#ifndef TSCH_CONF_ASSOCIATION_POLL_FREQUENCY
#define TSCH_CONF_ASSOCIATION_POLL_FREQUENCY 100
#endif

#ifndef TSCH_CONF_WAIT_FOR_EB
#define TSCH_WAIT_FOR_EB TSCH_CONF_WAIT_FOR_EB/*Time spent sending a 35B EB on SF12*/
#else 
#define TSCH_WAIT_FOR_EB RTIMER_SECOND * 2.2
#endif

#define USEC_SLEEP_TO_STANDBY 250
#define USEC_FREQ_SYNTH 60
#define USEC_STANDBY_TO_RX 71 
#define USEC_STANDBY_TO_TX 120
#define USEC_CHANNEL_HOP   20

/*Maybe we should note that the LoRa preambles are so slow that this doesn't matter much*/
#define RADIO_DELAY_BEFORE_TX_SX1272 US_TO_RTIMERTICKS(USEC_STANDBY_TO_TX + USEC_FREQ_SYNTH)//Did not find this in the documentation. Note, is always in RX mode when on. Assumes same flip for TX
#define RADIO_DELAY_BEFORE_RX_SX1272 US_TO_RTIMERTICKS(USEC_STANDBY_TO_RX + USEC_FREQ_SYNTH)//Table 22, datasheet.

#define RADIO_DELAY_BEFORE_TX RADIO_DELAY_BEFORE_TX_SX1272
#define RADIO_DELAY_BEFORE_RX RADIO_DELAY_BEFORE_RX_SX1272

#ifndef SX1272_CONF_RADIO_DELAY_BEFORE_DETECT
#define RADIO_DELAY_BEFORE_DETECT US_TO_RTIMERTICKS(6000)
#else
#define RADIO_DELAY_BEFORE_DETECT SX1272_CONF_RADIO_DELAY_BEFORE_DETECT
#endif
#define TSCH_CONF_RADIO_ON_DURING_TIMESLOT 0
/*We really want to do this, since the timestamps and RxDone-interrupts are much more accurate than the modemstatus detection*/
#define TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS 1


#endif /*USE_SX1272_AS_STANDARD_RADIO*/
#endif /*SX1272_PROJECT_CONF_MACROS*/
