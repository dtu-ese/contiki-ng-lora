#include "sx1272.h"
#include "sx1272-gpio-interface.h"
#include "sx1272-spi-interface.h"

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

void initialize_hardware(){

}

void initialize_software(){

}




