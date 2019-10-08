#include "sx1272.h"
#include "sx1272-gpio-interface.h"
#include "sx1272-spi-interface.h"
#include "sx1272-regs-lora.h"
#include "sx1272-main-driver.h"
#include "sys/energest.h"
#include "radio.h"
#include "dev/watchdog.h"

static const uint32_t channels[8] = LORA_CHANNELS;
#define RX_TX_BUFFER_SIZE 256
static uint8_t rx_tx_buffer[RX_TX_BUFFER_SIZE];
static uint8_t pending_packet_flag;
static uint8_t pending_packet_length;
static rtimer_clock_t last_packet_received;
static rtimer_clock_t time_at_rx_done;
static uint8_t prev_opmode;
static uint8_t tx_power;

void initialize_contiki_end();/*Initialize spi and gpio*/
void initialize_radio();/*Initializes the sx1272-radio*/
void initialize_software();/*Initializes this driver, last received packet etc*/
void general_setup();/*Does some radio setup, with the modem configuration registers. For now we only do this once. Maybe move it to init_software*/
void set_opmode(uint8_t opmode);/*Sets opmode, e.g. standby, sleep, tx etc. with LoRa. Built-in delays to compensate for radio spending time to go to the different modes*/
void read_opmode();/*Reads the current opmode*/

int init_sx1272(){
  initialize_contiki_end();
  initialize_radio();
  initialize_software();
  return RADIO_RESULT_OK;
}

void initialize_contiki_end(){
    gpio_init_sx1272();
    spi_init_sx1272();
}

void initialize_radio(){
    reset_device();
    spi_write_sx1272(REG_LR_OPMODE, RFLR_OPMODE_LONGRANGEMODE_ON);/*Sets the device in LoRa mode, and puts it to sleep*/
    set_channel(0);

    /*Handle TX Output power*/
    if (SX1272_TX_OUTPUT_POWER > 20){
      tx_power = 20;
    } else if (SX1272_TX_OUTPUT_POWER > 17){
      tx_power = 17;
    } else if (SX1272_TX_OUTPUT_POWER < -1){
      tx_power = -1;
    } else {
      tx_power = SX1272_TX_OUTPUT_POWER;
    }
    if (tx_power >= 20){/*PA_Boost with high power*/
      spi_write_sx1272(REG_LR_PACONFIG, (tx_power - 5 ) | RFLR_PACONFIG_PASELECT_PABOOST);
      spi_write_sx1272(REG_LR_PADAC, (spi_read_sx1272(REG_LR_PADAC) & RFLR_PADAC_20DBM_MASK) | (~RFLR_PADAC_20DBM_MASK | RFLR_PADAC_20DBM_ON));
    } else if (tx_power >= 15){/*PA_BOOST*/
      spi_write_sx1272(REG_LR_PACONFIG, (tx_power - 2 ) | RFLR_PACONFIG_PASELECT_PABOOST);
      spi_write_sx1272(REG_LR_PADAC, (spi_read_sx1272(REG_LR_PADAC) & RFLR_PADAC_20DBM_MASK) | (~RFLR_PADAC_20DBM_MASK | RFLR_PADAC_20DBM_OFF));
    } else {/*RFIO Pin*/
      spi_write_sx1272(REG_LR_PACONFIG, (tx_power + 1 ) | RFLR_PACONFIG_PASELECT_RFO);
      spi_write_sx1272(REG_LR_PADAC, (spi_read_sx1272(REG_LR_PADAC) & RFLR_PADAC_20DBM_MASK) | (~RFLR_PADAC_20DBM_MASK | RFLR_PADAC_20DBM_OFF));
    }

    general_setup();
}

/*Initializes last received packet et al*/
void initialize_software(){
  last_packet_received = 0;
  pending_packet_flag = 0;
  ENERGEST_ON(ENERGEST_TYPE_SX1272_SLEEP);
}

int get_current_rssi(){
  static int16_t rssi_val;
  rssi_val = -139 + spi_read_sx1272(REG_LR_RSSIVALUE);
  return rssi_val;
}

int get_last_snr(){
  return spi_read_sx1272(REG_LR_PKTSNRVALUE);  
}
int get_last_packet_rssi(){
  if (get_last_snr < 0){
    return (int) (-139 + spi_read_sx1272(RADIO_PARAM_LAST_RSSI)*0.25);
  } else {
    return (int) (-139 + spi_read_sx1272(RADIO_PARAM_LAST_RSSI) + get_last_snr()*0.25);
  }
}

uint8_t get_opmode(){
  prev_opmode = spi_read_sx1272(REG_LR_OPMODE);
  return prev_opmode & ~RFLR_OPMODE_MASK;
}

/*For these values, see doc. Standby to Rx assumes freq of at least 125 khz*/
#define USEC_SLEEP_TO_STANDBY 250
#define USEC_FREQ_SYNTH 60
#define USEC_STANDBY_TO_RX 71 
#define USEC_STANDBY_TO_TX 120
#define USEC_CHANNEL_HOP   20

void set_opmode(uint8_t opmode){
  /*Will always ensure LoRa Mode*/
  prev_opmode = get_opmode();
  
  if (prev_opmode == opmode){
    return;
  }

  spi_write_sx1272( REG_LR_OPMODE, (1 << 7) | (opmode & ~RFLR_OPMODE_MASK) );
  /*We 'wait' the time the switch takes*/
  switch(opmode){
    case RFLR_OPMODE_SLEEP:
    {
      /*We do nothing, assumed immediate switch*/
      switch (prev_opmode){
        case RFLR_OPMODE_STANDBY:
        {/*ENERGEST_TYPE_SX1272_TRANSMIT,ENERGEST_TYPE_SX1272_RX,ENERGEST_TYPE_SX1272_SLEEP,ENERGEST_TYPE_SX1272_STANDBY*/
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_STANDBY, ENERGEST_TYPE_SX1272_SLEEP);
        }
        case RFLR_OPMODE_TRANSMITTER:
        {
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_TRANSMIT, ENERGEST_TYPE_SX1272_SLEEP);
        }
        case RFLR_OPMODE_RECEIVER:
        {
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_RX, ENERGEST_TYPE_SX1272_SLEEP);
        }
        default:
        {

        }
      }
    }
    case RFLR_OPMODE_STANDBY:
    {
      switch (prev_opmode){
        case RFLR_OPMODE_SLEEP:
        {
          clock_delay_usec(USEC_SLEEP_TO_STANDBY);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_SLEEP, ENERGEST_TYPE_SX1272_STANDBY);
        }
        case RFLR_OPMODE_TRANSMITTER:
        {
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_TRANSMIT, ENERGEST_TYPE_SX1272_STANDBY);
        }
        case RFLR_OPMODE_RECEIVER:
        {
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_RX, ENERGEST_TYPE_SX1272_STANDBY);
        }
        default:
        {

        }
      }
    }
    case RFLR_OPMODE_RECEIVER:
    {
      switch (prev_opmode){
        case RFLR_OPMODE_SLEEP:
        {
          clock_delay_usec(USEC_SLEEP_TO_STANDBY + USEC_STANDBY_TO_RX + USEC_FREQ_SYNTH);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_SLEEP, ENERGEST_TYPE_SX1272_RX);          
        }
        case RFLR_OPMODE_STANDBY:
        {
          clock_delay_usec(USEC_STANDBY_TO_RX + USEC_FREQ_SYNTH);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_STANDBY, ENERGEST_TYPE_SX1272_RX);
        }
        /*This case should never occur, as we go from tx to standby always*/
        case RFLR_OPMODE_TRANSMITTER:
        {
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_TRANSMIT, ENERGEST_TYPE_SX1272_RX);
        }
        default:
        {

        }
      }
    }
    case RFLR_OPMODE_TRANSMITTER:
    {
      switch (prev_opmode){
        case RFLR_OPMODE_SLEEP:
        {
          clock_delay_usec(USEC_SLEEP_TO_STANDBY + USEC_STANDBY_TO_TX + USEC_FREQ_SYNTH);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_SLEEP, ENERGEST_TYPE_SX1272_TRANSMIT);
        }
        case RFLR_OPMODE_STANDBY:
        {
          clock_delay_usec(USEC_STANDBY_TO_TX + USEC_FREQ_SYNTH);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_STANDBY, ENERGEST_TYPE_SX1272_TRANSMIT);
        }
        case RFLR_OPMODE_RECEIVER:
        {
          clock_delay_usec(USEC_FREQ_SYNTH + USEC_CHANNEL_HOP);
          ENERGEST_SWITCH(ENERGEST_TYPE_SX1272_RX, ENERGEST_TYPE_SX1272_TRANSMIT);
        }
        default:
        {

        }
      }
    }
    default:
    {
      /*We do nothing*/
    }
  }

}

void general_setup(){
  static uint8_t modemconfig1;
  static uint8_t modemconfig2;

  set_opmode(RFLR_OPMODE_SLEEP);
  /*We don't do datarate optimization. CRC is handled by contiki-ng. Header needed for payload length*/
  modemconfig1 = ((SX1272_BANDWIDTH << 6) & ~RFLR_MODEMCONFIG1_BW_MASK) | ((SX1272_CODINGRATE << 3) & ~RFLR_MODEMCONFIG1_CODINGRATE_MASK) | RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF;
  /*Ensuring SF And turning AGC on so we don't have manual LNA settings*/
  modemconfig2 = ((SX1272_SPREADING_FACTOR << 4) & ~RFLR_MODEMCONFIG2_SF_MASK)| RFLR_MODEMCONFIG2_AGCAUTO_ON;
  spi_write_sx1272(REG_LR_MODEMCONFIG1, modemconfig1);
  spi_write_sx1272(REG_LR_MODEMCONFIG2, modemconfig2);
  /*Preamble length*/
  spi_write_sx1272(REG_LR_PREAMBLEMSB, (SX1272_PREAMBLE_LENGTH >> 8) & 0xFF);
  spi_write_sx1272(REG_LR_PREAMBLELSB, SX1272_PREAMBLE_LENGTH & 0xFF);
}

int prepare(const void *buffer, unsigned short size){
  set_opmode(RFLR_OPMODE_STANDBY);
  // Initializes the payload size
  spi_write_sx1272(REG_LR_PAYLOADLENGTH, size);

  // Full buffer used for Tx
  spi_write_sx1272(REG_LR_FIFOTXBASEADDR, 0);
  spi_write_sx1272(REG_LR_FIFOADDRPTR, 0);

  // FIFO operations can not take place in Sleep mode
  // Write payload buffer
  spi_write_fifo_sx1272(buffer, size);
  return RADIO_RESULT_OK;
}

int transmit(unsigned short size){
  set_opmode(RFLR_OPMODE_TRANSMITTER);
  clock_delay_usec(SX1272_PHY_OVERHEAD + SX1272_PAYLOAD_TIME(size));
  ENERGEST_SWITCH(ENERGEST_TYPE_TRANSMIT, ENERGEST_TYPE_CPU);/*As the radio automatically stops transmitting and go into standby when it's done tx'ing*/ 
  return 0;
}
int send(const void *buffer, unsigned short size){
  prepare(buffer, size);
  transmit(size);
  return 0;
}

/*This method ensures that packets are only picked up through polling, making timing better*/
int read_packet(void *buf, unsigned short buf_len){
  if (pending_packet_flag){
    memcpy(buf, rx_tx_buffer, pending_packet_length);
    pending_packet_flag = 0;
    return pending_packet_length;
  } else {
    return 0;
  }
}

int pending_packet(){
  
  receiving_packet_sx1272();
  return pending_packet_flag;
}

int rx_mode(){
  prev_opmode = get_opmode();

  if (prev_opmode == RFLR_OPMODE_RECEIVER){
    /*Clear Interrupt vector, sanity check*/
    spi_write_sx1272(REG_LR_IRQFLAGS, 0xf);
    return RADIO_RESULT_OK;
  }
  set_opmode(RFLR_OPMODE_STANDBY);
  spi_write_sx1272(REG_LR_FIFORXBASEADDR, 0);
  spi_write_sx1272(REG_LR_FIFOADDRPTR, 0);
  memset(rx_tx_buffer, 0, (size_t) RX_TX_BUFFER_SIZE);

  /*Clear Interrupt vector. Now is the only time we need it.*/
  spi_write_sx1272(REG_LR_IRQFLAGS, 0xf);

  /*Setup interrupts. Enable RxDone, disable everything else, as RxDone is the only one we poll*/
  spi_write_sx1272( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
          //RFLR_IRQFLAGS_RXDONE |
          RFLR_IRQFLAGS_PAYLOADCRCERROR |
          RFLR_IRQFLAGS_VALIDHEADER |
          RFLR_IRQFLAGS_TXDONE |
          RFLR_IRQFLAGS_CADDONE |
          RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
          RFLR_IRQFLAGS_CADDETECTED );
  // DIO0=RxDone. The only interrupt that needs firing. We do not use gpio-pin DIO0, we only read the interrupt register IRQFlags.
  spi_write_sx1272( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00);


  set_opmode(RFLR_OPMODE_RECEIVER);
  return RADIO_RESULT_OK;
}

int off_sx1272(){
  set_opmode(RFLR_OPMODE_SLEEP);
  return RADIO_RESULT_OK;
}


#define FREQ_STEP                                   61.03515625 /*This depends on the oscillator*/
int set_channel(uint8_t channel)
{
  static uint32_t freq;
  prev_opmode = get_opmode();
  if (prev_opmode != RFLR_OPMODE_SLEEP){
    /*We need to sleep or standby to change config registers*/
    set_opmode(RFLR_OPMODE_STANDBY);
  }
  freq = channels[(channel - 11) & 0x7];/*Takes mod 8, as only 8 LoRa channels are usd. While this is risky, this does work with all the TSCH channel sequences as of Sep 2019*/
  freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
  spi_write_sx1272( REG_LR_FRFMSB, ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
  spi_write_sx1272( REG_LR_FRFMID, ( uint8_t )( ( freq >> 8 ) & 0xFF ) );
  spi_write_sx1272( REG_LR_FRFLSB, ( uint8_t )( freq & 0xFF ) );
  /*We go back to where we were*/
  set_opmode(prev_opmode);
  return RADIO_RESULT_OK;
}

rtimer_clock_t time_of_last_packet_reception(){
  return last_packet_received;  
}


void storepacket(){
  pending_packet_length = spi_read_sx1272(REG_LR_RXNBBYTES);
  spi_read_fifo_sx1272(rx_tx_buffer, pending_packet_length);

  last_packet_received = time_at_rx_done - TSCH_PACKET_DURATION(pending_packet_length);
  pending_packet_flag = 1;
}

int receiving_packet_sx1272(){
    static uint8_t flags;
    static uint8_t status;
    flags = spi_read_sx1272(REG_LR_IRQFLAGS);
    status = spi_read_sx1272(REG_LR_MODEMSTAT);
    if (flags & RFLR_IRQFLAGS_RXDONE){
        /*We clear the flags*/
        spi_write_sx1272(REG_LR_IRQFLAGS, 0xFF);
        time_at_rx_done = RTIMER_NOW();
        storepacket();
        return 0;/*We are done receiving */
    }

  if (status & ( 8 | 2 | 1)){/*Checking against the status reg*/
    return 1;
  }
  return 0;
}
