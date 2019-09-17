/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX1272 driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "sx1272-radio.h"
#include "sx1272.h"
#include "sx1272-board.h"
#include "gpio-hal.h"
#include "dev/spi.h"
/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s SX1272Radio =
{
    SX1272Init,
    SX1272GetStatus,
    SX1272SetModem,
    SX1272SetChannel,
    SX1272IsChannelFree,
    SX1272Random,
    SX1272SetRxConfig,
    SX1272SetTxConfig,
    SX1272CheckRfFrequency,
    SX1272GetTimeOnAir,
    SX1272Send,
    SX1272SetSleep,
    SX1272SetStby, 
    SX1272SetRx,
    SX1272StartCad,
    SX1272ReadRssi,
    SX1272Write,
    SX1272Read,
    SX1272WriteBuffer,
    SX1272ReadBuffer,
    SX1272SetMaxPayloadLength
};

void SX1272IoInit( void )
{
    gpio_hal_arch_init();
    gpio_hal_arch_pin_set_output(SX1272.pins.ant_switch);
    gpio_hal_arch_pin_set_output(SX1272.pins.Reset);
    spi_acquire(&SX1272.pins.Spi);
}

/*
 * List of interrupts used for Gpio pins.
 */
static gpio_hal_event_handler_t interrupts[6];
/* -------------------------------------------------- */
void SX1272IoIrqInit( DioIrqHandler **irqHandlers )
{
    printf("Initializing io irq\n");
    SX1272IoInit();

    gpio_hal_pin_cfg_t cfg;

    //uint8_t diopins[2] = {0, 3};

    for (int j = 0; j < 0; j++){
        uint8_t i = j;
        cfg = GPIO_HAL_PIN_CFG_EDGE_RISING | GPIO_HAL_PIN_CFG_INT_ENABLE;
        gpio_hal_arch_pin_set_input(SX1272.pins.DIO[i]);
        gpio_hal_arch_pin_cfg_set(SX1272.pins.DIO[i], cfg);
        gpio_hal_arch_interrupt_enable(SX1272.pins.DIO[i]);

        interrupts[i].pin_mask = gpio_hal_pin_to_mask(SX1272.pins.DIO[i]);
        interrupts[i].handler = ((gpio_hal_callback_t) irqHandlers[i]);
        gpio_hal_register_handler(&interrupts[i]);        
    }
}

void SX1272IoDeInit( void )
{
    for (int i = 0; i < 6; i++){
        gpio_hal_arch_clear_pin(SX1272.pins.DIO[i]);
        gpio_hal_arch_interrupt_disable(SX1272.pins.DIO[i]);
    }
    SX1272AntSwDeInit();
    gpio_hal_arch_clear_pin(SX1272.pins.Reset);
    spi_release(&SX1272.pins.Spi);

}

uint8_t SX1272GetPaSelect( uint32_t channel )
{
    return RF_PACONFIG_PASELECT_PABOOST;
}

void SX1272SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;
    
        if( status == false )
        {
            SX1272AntSwInit( );
        }
        else
        {
            SX1272AntSwDeInit( );
        }
    }
}

void SX1272AntSwInit( void )
{
        gpio_hal_arch_write_pin( SX1272.pins.ant_switch, 0 );
}

void SX1272AntSwDeInit( void )
{
        gpio_hal_arch_write_pin( SX1272.pins.ant_switch, 0 );
}

void SX1272SetAntSw( uint8_t rxTx )
{
    if( rxTx != 0 ) // 1: TX, 0: RX
    {
        gpio_hal_arch_write_pin( SX1272.pins.ant_switch, 1 );
    }
    else
    {
        gpio_hal_arch_write_pin( SX1272.pins.ant_switch, 0 );
    }
}

bool SX1272CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
