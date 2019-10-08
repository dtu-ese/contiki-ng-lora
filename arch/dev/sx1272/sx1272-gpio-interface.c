#include "contiki.h"
#include "dev/gpio-hal.h"
#include "sx1272.h"
#include "sx1272-gpio-interface.h"

/*The spi pins are merely an spi_device_t struct. Pin selection should be done through defines, referred to above.*/
struct gpio_pins_sx1272_t {
    gpio_hal_port_t       reset_port;
    gpio_hal_pin_t        reset;
};
static const struct gpio_pins_sx1272_t sx1272_gpio_pins = {
  SX1272_RESET_PIN_PORT,
  SX1272_RESET_PIN,
};

static void turn_on_reset_pin(){
  gpio_hal_arch_set_pin(sx1272_gpio_pins.reset_port, sx1272_gpio_pins.reset);
}

static void turn_off_reset_pin(){
  gpio_hal_arch_clear_pin(sx1272_gpio_pins.reset_port, sx1272_gpio_pins.reset);  
}



void gpio_init_sx1272(){
  gpio_hal_init();
  gpio_hal_arch_pin_set_output(sx1272_gpio_pins.reset_port, sx1272_gpio_pins.reset);
  /*Ensure that the reset pin is not on*/
  turn_off_reset_pin();
}

void reset_device(){
    /*According to documentation, reset pin is down for at least 100us, and then wait for 5ms to do a reset*/
    turn_on_reset_pin();
    rtimer_clock_t deadline = RTIMER_NOW() + US_TO_RTIMERTICKS(100)+1;
    while (RTIMER_CLOCK_LT(RTIMER_NOW(), deadline));

    turn_off_reset_pin();
    deadline = RTIMER_NOW() + US_TO_RTIMERTICKS(5000)+1;
    while (RTIMER_CLOCK_LT(RTIMER_NOW(), deadline));
}



