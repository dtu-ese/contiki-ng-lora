#include "contiki.h"
#include "radio.h"
#include "sx1272.h"
#include "sx1272-regs-lora.h"
#include "sx1272-main-driver.h"
#include "rtimer.h"
#include "net/mac/tsch/tsch.h"

#define BUFFER_SIZE 256

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
    *value = get_current_rssi();
    return RADIO_RESULT_OK;
  case RADIO_PARAM_LAST_RSSI:
    *value = get_last_packet_rssi();
    return RADIO_RESULT_OK;
  case RADIO_PARAM_LAST_LINK_QUALITY:
    *value = get_last_snr();
    return RADIO_RESULT_OK;
  case RADIO_CONST_MAX_PAYLOAD_LEN:
    *value = 150;/*We pick this ourselves, but max is 255. Most likely the hard limit is bound by your timeslots.*/
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
      set_channel(value);
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
    *((rtimer_clock_t *) dest) = time_of_last_packet_reception();
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

#define CCA_CLEAR 1
#define CCA_BUSY 0
int clear_channel_assesment(){
  return CCA_CLEAR;/*CCA In LoRA is slow, so we don't do it*/
}


const struct radio_driver sx1272_radio_driver = {
  init_sx1272,
  prepare,
  transmit,
  send,
  read_packet,
  clear_channel_assesment,
  receiving_packet_sx1272,
  pending_packet,
  rx_mode,
  off_sx1272,
  get_value_sx1272,
  set_value_sx1272,
  get_object_sx1272,
  set_object_sx1272,
};