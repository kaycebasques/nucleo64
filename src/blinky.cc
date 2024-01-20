#include <cstddef>

#include "libopencm3/stm32/f4/rcc.h"
#include "libopencm3/stm32/f4/gpio.h"

#define LED_PORT (GPIOA)
#define LED_PIN  (GPIO5)

static void rcc_setup(void) {
  rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
}

static void gpio_setup(void) {
  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void delay_cycles(uint32_t cycles) {
  for (uint32_t i = 0; i < cycles; i++) {
    __asm__("nop");
  }
}

int main() {
  rcc_setup();
  gpio_setup();
  while (true) {
    gpio_toggle(LED_PORT, LED_PIN);
    delay_cycles(84000000);
  }
  return 0;
}
