/*
 * Copyright (c) 2015, SICS Swedish ICT.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
/*Configuration of LoRa radio driver*/

#define USE_SX1272_AS_STANDARD_RADIO    1
#define SX1272_CONF_TX_OUTPUT_POWER     14
#define SX1272_CONF_SPREADING_FACTOR    7
#define SX1272_CONF_CODINGRATE          1
#define SX1272_CONF_PREAMBLE_LENGTH     6
#define SX1272_CONF_TX_SYMBOL_TIMEOUT   5
#define SX1272_CONF_BANDWIDTH           0
#define SX1272_CONF_SPI_CONTROLLER      0
#define SX1272_CONF_PINS
#define SX1272_CONF_RESET_PIN  BOARD_IOID_DIO15
#define SX1272_CONF_SPI_SCK EXT_FLASH_SPI_PIN_SCK
#define SX1272_CONF_SPI_MISO EXT_FLASH_SPI_PIN_MISO
#define SX1272_CONF_SPI_MOSI EXT_FLASH_SPI_PIN_MOSI
#define SX1272_CONF_SPI_CS EXT_FLASH_SPI_PIN_CS
#define SX1272_CONF_SPI_BITRATE 1000000
#define TSCH_CONF_WAIT_FOR_EB   RTIMER_SECOND/0.1 /*Sending an EB is 70ms*/

/*We need these macros to be imported earli in the build process*/
#define TSCH_CONF_ASSOCIATION_POLL_FREQUENCY 32
#include "dev/sx1272/sx1272-project-conf-macros.h"

/* Set to enable TSCH security */
#ifndef WITH_SECURITY
#define WITH_SECURITY 0
#endif /* WITH_SECURITY */

/* USB serial takes space, free more space elsewhere */
#define SICSLOWPAN_CONF_FRAG 0
#define UIP_CONF_BUFFER_SIZE 160


/* Length of the various slotframes. Tune to balance network capacity,
 * contention, energy, latency. */
#define ORCHESTRA_CONF_EBSF_PERIOD 11

#define ORCHESTRA_CONF_COMMON_SHARED_PERIOD 7

#define ORCHESTRA_CONF_UNICAST_PERIOD 13

/* Is the per-neighbor unicast slotframe sender-based (if not, it is receiver-based).
 * Note: sender-based works only with RPL storing mode as it relies on DAO and
 * routing entries to keep track of children and parents. */
#define ORCHESTRA_CONF_UNICAST_SENDER_BASED 1


/*******************************************************/
/******************* Configure TSCH ********************/
/*******************************************************/

#define PACKETBUF_CONF_SIZE 128

/* IEEE802.15.4 PANID */
#define IEEE802154_CONF_PANID 0x81a5

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#define TSCH_CONF_AUTOSTART 0

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 6
#define CONTIKI_WATCHDOG_CONF_TIMER_TOP 0xFFFFFF

#if WITH_SECURITY

/* Enable security */
#define LLSEC802154_CONF_ENABLED 1

#endif /* WITH_SECURITY */

/*******************************************************/
/************* Other system configuration **************/
/*******************************************************/

/* Logging */
#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_INFO
#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_NONE
#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_NONE
#define TSCH_LOG_CONF_PER_SLOT                     1

#define RPL_CONF_DAO_RETRANSMISSION_TIMEOUT 10*CLOCK_SECOND
#define RPL_CONF_DAO_MAX_RETRANSMISSIONS 15
#define RPL_CONF_DIO_INTERVAL_MIN 15
#define RPL_CONF_DIO_INTERVAL_DOUBLINGS 2
#define NETSTACK_MAX_ROUTE_ENTRIES 5

#define ENERGEST_CONF_ON 1

/********** Includes **********/
//#define BUILD_WITH_ORCHESTRA                            1
// #define ORCHESTRA_CONF_EBSF_PERIOD                     23

// #define ORCHESTRA_CONF_COMMON_SHARED_PERIOD            13

// #define ORCHESTRA_CONF_UNICAST_PERIOD                  17

//#include "contiki.h"

/******** Configuration: synchronization *******/

/* Max time before sending a unicast keep-alive message to the time source */
#define TSCH_CONF_KEEPALIVE_TIMEOUT (CLOCK_SECOND*240)

/* With TSCH_ADAPTIVE_TIMESYNC enabled: keep-alive timeout used after reaching
 * accurate drift compensation. */
#define TSCH_CONF_MAX_KEEPALIVE_TIMEOUT (CLOCK_SECOND*240)

/* Max time without synchronization before leaving the PAN */
#define TSCH_CONF_DESYNC_THRESHOLD (CLOCK_SECOND*360)

/* Period between two consecutive EBs */
#define TSCH_CONF_EB_PERIOD (CLOCK_SECOND*30)

/* Max Period between two consecutive EBs */
#define TSCH_CONF_MAX_EB_PERIOD (40*CLOCK_SECOND)
#define TSCH_CONF_MIN_EB_PERIOD (20*CLOCK_SECOND)
#define TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS 1

/* Use SFD timestamp for synchronization? By default we merely rely on rtimer and busy wait
 * until SFD is high, which we found to provide greater accuracy on JN516x and CC2420.
 * Note: for association, however, we always use SFD timestamp to know the time of arrival
 * of the EB (because we do not busy-wait for the whole scanning process)
 * */
/*
#ifdef TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS
#define TSCH_RESYNC_WITH_SFD_TIMESTAMPS TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS
#else
define TSCH_RESYNC_WITH_SFD_TIMESTAMPS 0
#endif
*/
/* If enabled, remove jitter due to measurement errors */
/*
#ifdef TSCH_CONF_TIMESYNC_REMOVE_JITTER
#define TSCH_TIMESYNC_REMOVE_JITTER TSCH_CONF_TIMESYNC_REMOVE_JITTER
#else
#define TSCH_TIMESYNC_REMOVE_JITTER TSCH_RESYNC_WITH_SFD_TIMESTAMPS
#endif
*/
/* Base drift value.
 * Used to compensate locally know inaccuracies, such as
 * the effect of having a binary 32.768 kHz timer as the TSCH time base. */
/*
#ifdef TSCH_CONF_BASE_DRIFT_PPM
#define TSCH_BASE_DRIFT_PPM TSCH_CONF_BASE_DRIFT_PPM
#else
#define TSCH_BASE_DRIFT_PPM 0
#endif
*/

/* Estimate the drift of the time-source neighbor and compensate for it? */
/*
#ifdef TSCH_CONF_ADAPTIVE_TIMESYNC
#define TSCH_ADAPTIVE_TIMESYNC TSCH_CONF_ADAPTIVE_TIMESYNC
#else
#define TSCH_ADAPTIVE_TIMESYNC 1
#endif
*/

/* An ad-hoc mechanism to have TSCH select its time source without the
 * help of an upper-layer, simply by collecting statistics on received
 * EBs and their join priority. Disabled by default as we recomment
 * mapping the time source on the RPL preferred parent
 * (via tsch_rpl_callback_parent_switch) */
/*
#ifdef TSCH_CONF_AUTOSELECT_TIME_SOURCE
#define TSCH_AUTOSELECT_TIME_SOURCE TSCH_CONF_AUTOSELECT_TIME_SOURCE
#else
#define TSCH_AUTOSELECT_TIME_SOURCE 0
#endif *//* TSCH_CONF_EB_AUTOSELECT */

/******** Configuration: channel hopping *******/

/* Default hopping sequence, used in case hopping sequence ID == 0 */
#define TSCH_CONF_DEFAULT_HOPPING_SEQUENCE TSCH_HOPPING_SEQUENCE_4_4

/* Hopping sequence used for joining (scan channels) */
/*
#ifdef TSCH_CONF_JOIN_HOPPING_SEQUENCE
#define TSCH_JOIN_HOPPING_SEQUENCE TSCH_CONF_JOIN_HOPPING_SEQUENCE
#else
#define TSCH_JOIN_HOPPING_SEQUENCE TSCH_DEFAULT_HOPPING_SEQUENCE
#endif
*/

/* Maximum length of the TSCH channel hopping sequence. Must be greater or
 * equal to the length of TSCH_DEFAULT_HOPPING_SEQUENCE. */
/*
#ifdef TSCH_CONF_HOPPING_SEQUENCE_MAX_LEN
#define TSCH_HOPPING_SEQUENCE_MAX_LEN TSCH_CONF_HOPPING_SEQUENCE_MAX_LEN
#else
#define TSCH_HOPPING_SEQUENCE_MAX_LEN sizeof(TSCH_DEFAULT_HOPPING_SEQUENCE)
#endif
*/
/******** Configuration: association *******/

/* Start TSCH automatically after init? If not, the upper layers
 * must call NETSTACK_MAC.on() to start it. Useful when the
 * application needs to control when the nodes are to start
 * scanning or advertising.*/
/*
#ifdef TSCH_CONF_AUTOSTART
#define TSCH_AUTOSTART TSCH_CONF_AUTOSTART
#else
#define TSCH_AUTOSTART 1
#endif
*/

/* Max acceptable join priority */
/*
#ifdef TSCH_CONF_MAX_JOIN_PRIORITY
#define TSCH_MAX_JOIN_PRIORITY TSCH_CONF_MAX_JOIN_PRIORITY
#else
#define TSCH_MAX_JOIN_PRIORITY 32
#endif
*/
/* Join only secured networks? (discard EBs with security disabled) */
/*
#ifdef TSCH_CONF_JOIN_SECURED_ONLY
#define TSCH_JOIN_SECURED_ONLY TSCH_CONF_JOIN_SECURED_ONLY
#else
*/
/* By default, set if LLSEC802154_ENABLED is also non-zero */
/*
#define TSCH_JOIN_SECURED_ONLY LLSEC802154_ENABLED
#endif
*/
/* By default, join any PAN ID. Otherwise, wait for an EB from IEEE802154_PANID */
/*
#ifdef TSCH_CONF_JOIN_MY_PANID_ONLY
#define TSCH_JOIN_MY_PANID_ONLY TSCH_CONF_JOIN_MY_PANID_ONLY
#else
#define TSCH_JOIN_MY_PANID_ONLY 1
#endif
*/
/* The radio polling frequency (in Hz) during association process */
/*
#ifdef TSCH_CONF_ASSOCIATION_POLL_FREQUENCY
#define TSCH_ASSOCIATION_POLL_FREQUENCY TSCH_CONF_ASSOCIATION_POLL_FREQUENCY
#else
#define TSCH_ASSOCIATION_POLL_FREQUENCY 100
#endif
*/
/* When associating, check ASN against our own uptime (time in minutes)..
 * Useful to force joining only with nodes started roughly at the same time.
 * Set to the max number of minutes acceptable. */
/*
#ifdef TSCH_CONF_CHECK_TIME_AT_ASSOCIATION
#define TSCH_CHECK_TIME_AT_ASSOCIATION TSCH_CONF_CHECK_TIME_AT_ASSOCIATION
#else
#define TSCH_CHECK_TIME_AT_ASSOCIATION 0
#endif
*/
/* By default: initialize schedule from EB when associating, using the
 * slotframe and links Information Element */
/*
#ifdef TSCH_CONF_INIT_SCHEDULE_FROM_EB
#define TSCH_INIT_SCHEDULE_FROM_EB TSCH_CONF_INIT_SCHEDULE_FROM_EB
#else
#define TSCH_INIT_SCHEDULE_FROM_EB 1
#endif
*/
/* How long to scan each channel in the scanning phase */
/*
#ifdef TSCH_CONF_CHANNEL_SCAN_DURATION
#define TSCH_CHANNEL_SCAN_DURATION TSCH_CONF_CHANNEL_SCAN_DURATION
#else
#define TSCH_CHANNEL_SCAN_DURATION CLOCK_SECOND
#endif
*/
/* TSCH EB: include timeslot timing Information Element? */
/*
#ifdef TSCH_PACKET_CONF_EB_WITH_TIMESLOT_TIMING
#define TSCH_PACKET_EB_WITH_TIMESLOT_TIMING TSCH_PACKET_CONF_EB_WITH_TIMESLOT_TIMING
#else
#define TSCH_PACKET_EB_WITH_TIMESLOT_TIMING 0
#endif
*/
/* TSCH EB: include hopping sequence Information Element? */
/*
#ifdef TSCH_PACKET_CONF_EB_WITH_HOPPING_SEQUENCE
#define TSCH_PACKET_EB_WITH_HOPPING_SEQUENCE TSCH_PACKET_CONF_EB_WITH_HOPPING_SEQUENCE
#else
#define TSCH_PACKET_EB_WITH_HOPPING_SEQUENCE 0
#endif
*/
/* TSCH EB: include slotframe and link Information Element? */
/*
#ifdef TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK
#define TSCH_PACKET_EB_WITH_SLOTFRAME_AND_LINK TSCH_PACKET_CONF_EB_WITH_SLOTFRAME_AND_LINK
#else
#define TSCH_PACKET_EB_WITH_SLOTFRAME_AND_LINK 0
#endif
*/
/******** Configuration: queues  *******/

/* Size of the ring buffer storing dequeued outgoing packets (only an array of pointers).
 * Must be power of two, and greater or equal to QUEUEBUF_NUM */
/*
#ifdef TSCH_CONF_DEQUEUED_ARRAY_SIZE
#define TSCH_DEQUEUED_ARRAY_SIZE TSCH_CONF_DEQUEUED_ARRAY_SIZE
#else
*/

/* Size of the ring buffer storing incoming packets.
 * Must be power of two */
/*
#ifdef TSCH_CONF_MAX_INCOMING_PACKETS
#define TSCH_MAX_INCOMING_PACKETS TSCH_CONF_MAX_INCOMING_PACKETS
#else
#define TSCH_MAX_INCOMING_PACKETS 4
#endif
*/
/* The maximum number of outgoing packets towards each neighbor
 * Must be power of two to enable atomic ringbuf operations.
 * Note: the total number of outgoing packets in the system (for
 * all neighbors) is defined via QUEUEBUF_CONF_NUM */
/*
#ifdef TSCH_QUEUE_CONF_NUM_PER_NEIGHBOR
#define TSCH_QUEUE_NUM_PER_NEIGHBOR TSCH_QUEUE_CONF_NUM_PER_NEIGHBOR
#else
*/

/* The number of neighbor queues. There are two queues allocated at all times:
 * one for EBs, one for broadcasts. Other queues are for unicast to neighbors */
/*
#ifdef TSCH_QUEUE_CONF_MAX_NEIGHBOR_QUEUES
#define TSCH_QUEUE_MAX_NEIGHBOR_QUEUES TSCH_QUEUE_CONF_MAX_NEIGHBOR_QUEUES
#else
#define TSCH_QUEUE_MAX_NEIGHBOR_QUEUES ((NBR_TABLE_CONF_MAX_NEIGHBORS) + 2)
#endif
*/
/******** Configuration: scheduling  *******/

/* Initializes TSCH with a 6TiSCH minimal schedule */
/*
#ifdef TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL
#define TSCH_SCHEDULE_WITH_6TISCH_MINIMAL TSCH_SCHEDULE_CONF_WITH_6TISCH_MINIMAL
#else
#define TSCH_SCHEDULE_WITH_6TISCH_MINIMAL (!(BUILD_WITH_ORCHESTRA))
#endif
*/

/* Set an upper bound on burst length. Set to 0 to never set the frame pending
 * bit, i.e., never trigger a burst. Note that receiver-side support for burst
 * is always enabled, as it is part of IEEE 802.1.5.4-2015 (Section 7.2.1.3)*/
#define TSCH_CONF_BURST_MAX_LEN 4

/* 6TiSCH Minimal schedule slotframe length */
/*
#ifdef TSCH_SCHEDULE_CONF_DEFAULT_LENGTH
#define TSCH_SCHEDULE_DEFAULT_LENGTH TSCH_SCHEDULE_CONF_DEFAULT_LENGTH
#else
#define TSCH_SCHEDULE_DEFAULT_LENGTH 7
#endif
*/

/* Max number of TSCH slotframes */
/*
#ifdef TSCH_SCHEDULE_CONF_MAX_SLOTFRAMES
#define TSCH_SCHEDULE_MAX_SLOTFRAMES TSCH_SCHEDULE_CONF_MAX_SLOTFRAMES
#else
#define TSCH_SCHEDULE_MAX_SLOTFRAMES 4
#endif
*/

/* Max number of links */
/*
#ifdef TSCH_SCHEDULE_CONF_MAX_LINKS
#define TSCH_SCHEDULE_MAX_LINKS TSCH_SCHEDULE_CONF_MAX_LINKS
#else
#define TSCH_SCHEDULE_MAX_LINKS 32
#endif
*/

/* To include Sixtop Implementation */
/*
#ifdef TSCH_CONF_WITH_SIXTOP
#define TSCH_WITH_SIXTOP TSCH_CONF_WITH_SIXTOP
#else
#define TSCH_WITH_SIXTOP 0
#endif
*/

/******** Configuration: hardware-specific settings *******/

/* HW frame filtering enabled */
#define TSCH_CONF_HW_FRAME_FILTERING 0


/* Keep radio always on within TSCH timeslot (1) or turn it off between packet and ACK? (0) */
//#define TSCH_CONF_RADIO_ON_DURING_TIMESLOT 0


/* TSCH timeslot timing template */
//#define TSCH_CONF_DEFAULT_TIMESLOT_TIMING tsch_timeslot_timing_us_10000

/* Configurable Rx guard time is micro-seconds */
//#define TSCH_CONF_RX_WAIT 22000

#endif /* __TSCH_CONF_H__ */
/** @} */





