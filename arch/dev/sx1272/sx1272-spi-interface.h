#ifndef SX1272_SPI_INTERFACE_H_
#define SX1272_SPI_INTERFACE_H_
#include "contiki.h"
#include "dev/spi.h"
#include "sx1272.h"

void spi_init_sx1272();/*Initialize spi connection*/
void spi_write_sx1272(uint8_t addr, uint8_t data);
uint8_t spi_read_sx1272(uint8_t addr);
void spi_write_buffer_sx1272(uint8_t addr, const uint8_t *buffer, uint8_t size);
void spi_read_buffer_sx1272(uint8_t addr, uint8_t *buffer, uint8_t size);
void spi_write_fifo_sx1272(const uint8_t *buffer, uint8_t size );
void spi_read_fifo_sx1272(uint8_t *buffer, uint8_t size);

#endif