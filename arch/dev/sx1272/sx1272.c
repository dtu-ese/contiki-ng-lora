#include "contiki.h"
#include "radio.h"
#include "sx1272.h"
#include "sx1272Regs-LoRa.h"
#include "sys/energest.h"

#include "rtimer.h"

#define CCA_CLEAR 1
#define CCA_BUSY 0

#define BUFFER_SIZE 256

uint16_t RxBufferSize = 0;
uint8_t RxBuffer[BUFFER_SIZE];
static int pending_packet_flag = 0;

int8_t RssiValue = 0;
int8_t SnrValue = 0;
static const SX1272pins_t sx1272pins = {
  SX1272_RESET_PIN_PORT,
  SX1272_ANT_SWITCH_PIN_PORT,
  SX1272_RESET_PIN,
  SX1272_ANT_SWITCH,
  {
    #ifdef GPIO_PORT_PIN_NUMBERING
      SX1272_SPI_SCK_PORT,
      SX1272_SPI_MISO_PORT,
      SX1272_SPI_MOSI_PORT,
      SX1272_SPI_CS_PORT,
    #endif
    SX1272_SPI_SCK,
    SX1272_SPI_MISO,
    SX1272_SPI_MOSI,
    SX1272_SPI_CS,
    SX1272_SPI_BITRATE,
    SX1272_SPI_PHASE,
    SX1272_SPI_POL,
    SX1272_SPI_CONTROLLER
  }
};
int reinit_sx1272(void){

    SX1272ReInit();
    SX1272Radio.SetChannel( RF_FREQUENCY );

    /*Set default configuration*/
    SX1272Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    SX1272Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

    return 1;
}

int init_sx1272(void){

    /*Tsch is not event-based*/

    
    SX1272Radio.SetChannel( RF_FREQUENCY );

    /*Set default configuration*/
    SX1272Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

    SX1272Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
    return 1;
}

uint8_t tx_buffer[128];
  /** Prepare the radio with a packet to be sent. */
int prepare_sx1272(const void *payload, unsigned short payload_len){
  //printf("Preparing\n");
  memcpy(tx_buffer, payload, payload_len);
  SX1272Prepare(tx_buffer, payload_len);
  return 0;
}

  /** Send the packet that has previously been prepared. */
int transmit_sx1272(unsigned short transmit_len){
  //printf("Sending\n");
  rtimer_clock_t waitTimer = RTIMER_NOW();
      ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);
  SX1272Transmit( transmit_len );
  rtimer_clock_t goalTime = waitTimer + TSCH_PACKET_DURATION_SX1272(transmit_len) + 1;

  /*We busy wait for transmission to complete */
  while(RTIMER_CLOCK_LT(RTIMER_NOW(), goalTime));
ENERGEST_SWITCH(ENERGEST_TYPE_TRANSMIT, ENERGEST_TYPE_LISTEN);

  return RADIO_TX_OK;
}

  /** Prepare & transmit a packet. */
   /*We will not need this for tsch*/
int send_sx1272(const void *payload, unsigned short payload_len){
  return 0;
}

  /** Read a received packet into a buffer. */
int read_sx1272(void *buf, unsigned short buf_len){
    //printf("! read?\n");

  if (pending_packet_flag | SX1272_tsch_driver.pending_packet()){
    //printf("! pending_packet?\n");
    /*Our buffer is too short to handle the data*/
    if (buf_len < RxBufferSize){
      printf("Buffer too short\n");
      return 0;
    }
    memcpy(buf, RxBuffer, buf_len);
    pending_packet_flag = 0;
    return RxBufferSize;
  } else {
    /*There is nothing waiting */
    return 0;
  }
}

  /** Perform a Clear-Channel Assessment (CCA) to find out if there is
      a packet in the air or not. */
int channel_clear_sx1272(void){
  if (SX1272ReadRssi(MODEM_LORA) > 60){
    return CCA_BUSY;
  } else {
    return CCA_CLEAR;
  } 
}

int getRSSI(uint16_t rssi, uint8_t snr){
  if (snr < 0){
    return -139 + rssi;
  } else {
    return -139 + rssi + snr*0.25;
  }
}
  /** Check if the radio driver is currently receiving a packet */
int receiving_packet_sx1272(void){
   /*Stuff is happening */
  return SX1272isReceiving();
}

  /** Turn the radio on. We will be in receiver mode.*/
int on_sx1272(void){
  uint8_t opmode = SX1272Read(REG_LR_OPMODE);
  //printf("opmode: %d\n", opmode);
  if (((opmode & ~RFLR_OPMODE_MASK) == RFLR_OPMODE_RECEIVER) && ((opmode & RF_OPMODE_LONGRANGEMODE_ON )== RF_OPMODE_LONGRANGEMODE_ON)){
    return 1;
  }
  on = 1;
  ENERGEST_ON(ENERGEST_TYPE_LISTEN);
  SX1272SetRx(0);
  //clock_delay_usec(1000);
  //printf("Turning on\n");
  //static struct timer t;
  //  timer_set(&t, 2);
  //  while (!timer_expired(&t));
  opmode = SX1272Read(REG_LR_OPMODE);
  if (!((opmode & RFLR_OPMODE_MASK) == RF_OPMODE_LONGRANGEMODE_ON)){
    printf("opmode: %d\n", opmode);
  }
  if (!(opmode & RF_OPMODE_LONGRANGEMODE_ON)){
    reinit_sx1272();
  }
//  printf("opmoderec: %d, opmodeLora: %d\n, total expression: %d", ((opmode & ~RFLR_OPMODE_MASK) == RFLR_OPMODE_RECEIVER), ((opmode & RF_OPMODE_LONGRANGEMODE_ON )== RF_OPMODE_LONGRANGEMODE_ON), ((opmode & ~RFLR_OPMODE_MASK) == RFLR_OPMODE_RECEIVER) && ((opmode & RF_OPMODE_LONGRANGEMODE_ON )== RF_OPMODE_LONGRANGEMODE_ON));
  //pending_packet_flag = 0;/*Clean slate. Note, we can only have one pending packet at the time atm.*/
  return 1;
}

  /** Turn the radio off. This clears FIFO-buffer. */

int off_sx1272(void){
  on = 0;
  /*We go to sleep*/
  SX1272PollPacket();/*Ensure we don't lose irqflags */
    ENERGEST_OFF(ENERGEST_TYPE_LISTEN);
  SX1272SetOpMode( RF_OPMODE_SLEEP );
  return 1;
}

  /** Check if the radio driver has just received a packet */
int pending_packet_sx1272(void){
            //printf("! pending?\n");

  if( pending_packet_flag){
    return pending_packet_flag;
  } else {
    return SX1272PollPacket();
  }
}




  /** Get a radio parameter value. */
radio_result_t get_value_sx1272(radio_param_t param, radio_value_t *value){
  if(!value) {
    return RADIO_RESULT_INVALID_VALUE;
  }

  switch(param) {
  case RADIO_PARAM_RX_MODE:
    *value = 0;
    return RADIO_RESULT_OK;
  case RADIO_PARAM_TX_MODE:
    *value = 0;
    return RADIO_RESULT_OK;
  case RADIO_PARAM_RSSI:
    SX1272SetOpMode( RF_OPMODE_RECEIVER );
    clock_delay_usec(1000);
    *value = getRSSI(SX1272ReadRssi(MODEM_LORA), 0);
    return RADIO_RESULT_OK;
  case RADIO_PARAM_LAST_RSSI:
    *value = getRSSI(SX1272Read(REG_LR_PKTRSSIVALUE), SX1272Read(REG_LR_PKTSNRVALUE));
    return RADIO_RESULT_OK;
  case RADIO_PARAM_LAST_LINK_QUALITY:
    *value = SX1272Read(REG_LR_PKTSNRVALUE);
    return RADIO_RESULT_OK;
  default:
    return RADIO_RESULT_NOT_SUPPORTED;
  }
}

  /** Set a radio parameter value. */
radio_result_t set_value_sx1272(radio_param_t param, radio_value_t value){
  switch(param){
    case RADIO_PARAM_CHANNEL:
    {
      SX1272SetOpMode(RF_OPMODE_STANDBY);
      switch(value)
      {
        /*Crude simplification, pointing some TSCH-channels to LoRa-channels*/
        //clock_delay_usec(250);
        case 15:        
//          printf("Setting channel 8681\n");
          SX1272SetChannel(CHANNEL1);
          break;
        
        case 20:        
//          printf("Setting channel 8683\n");
          SX1272SetChannel(CHANNEL2);
          break;
        
        case 26:        
//          printf("Setting channel 8685\n");
          SX1272SetChannel( CHANNEL3 );
          break;
        
        case 25:        
//          printf("Setting channel 8671\n");
          SX1272SetChannel( CHANNEL4 );
          break;
        
        default:
        
          return RADIO_RESULT_OK;
          break;

        
      }
      return RADIO_RESULT_OK;
      break;
    }

    case RADIO_PARAM_RX_MODE:
    {
      return RADIO_RESULT_OK;
    }

    /*We are only in tx mode when send is called*/
    case RADIO_PARAM_TX_MODE:
    {
      return RADIO_RESULT_OK;
    }

    /*We are in rx mode at all times send not is called and the radio is turned on*/
  default:
    return RADIO_RESULT_NOT_SUPPORTED;
  }
  return RADIO_RESULT_NOT_SUPPORTED;
}

  /**
   * Get a radio parameter object. The argument 'dest' must point to a
   * memory area of at least 'size' bytes, and this memory area will
   * contain the parameter object if the function succeeds.
   */
radio_result_t get_object_sx1272(radio_param_t param, void *dest, size_t size){
  if(!dest) {
    return RADIO_RESULT_INVALID_VALUE;
  }

  switch(param) {
  case RADIO_PARAM_LAST_PACKET_TIMESTAMP:
    *((rtimer_clock_t *) dest) = SX1272_getLastPacketTimestamp();
    //printf("Last packet timestamp: %lld\n", (long long int ) *((rtimer_clock_t *) dest));
    //printf("Last packet timestamp Call: %lld\n", (long long int ) SX1272_getLastPacketTimestamp());
    return RADIO_RESULT_OK;
  default:
    return RADIO_RESULT_NOT_SUPPORTED;
  }
}

  /**
   * Set a radio parameter object. The memory area referred to by the
   * argument 'src' will not be accessed after the function returns.
   */
   /*We will not need this for tsch*/
radio_result_t set_object_sx1272(radio_param_t param, const void *src, size_t size){
  return RADIO_RESULT_OK;
}

const struct radio_driver SX1272_radio_driver = {
  init_sx1272,
  prepare_sx1272,
  transmit_sx1272,
  send_sx1272,
  read_sx1272,
  channel_clear_sx1272,
  receiving_packet_sx1272,
  pending_packet_sx1272,
  on_sx1272,
  off_sx1272,
  get_value_sx1272,
  set_value_sx1272,
  get_object_sx1272,
  set_object_sx1272,
};