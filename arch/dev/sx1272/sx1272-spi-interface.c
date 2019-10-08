
#include "contiki.h"
#include "dev/spi.h"
#include "sx1272-spi-interface.h"
static const spi_device_t sx1272_spi = {
    #if GPIO_PORT_PIN_NUMBERING
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
};

void spi_init_sx1272(){
    spi_acquire(&sx1272_spi);
}

void spi_write_sx1272( uint8_t addr, uint8_t data )
{
    spi_write_buffer_sx1272( addr, &data, 1 );
}

uint8_t spi_read_sx1272( uint8_t addr )
{
    uint8_t data;
    spi_read_buffer_sx1272( addr, &data, 1 );
    return data;
}

void spi_write_buffer_sx1272( uint8_t addr, const uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    spi_select(&sx1272_spi);
    spi_write_byte( &sx1272_spi, addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        spi_write_byte( &sx1272_spi, buffer[i] );
    }
    spi_deselect(&sx1272_spi);

}

void spi_read_buffer_sx1272( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
    
    spi_select(&sx1272_spi);
    spi_write_byte( &sx1272_spi, addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        spi_read_byte( &sx1272_spi, &buffer[i] );
    }
    spi_deselect(&sx1272_spi);
}

void spi_write_fifo_sx1272(const uint8_t *buffer, uint8_t size )
{
    spi_write_buffer_sx1272( 0, buffer, size );
}

void spi_read_fifo_sx1272( uint8_t *buffer, uint8_t size )
{
    spi_read_buffer_sx1272( 0, buffer, size );
}
