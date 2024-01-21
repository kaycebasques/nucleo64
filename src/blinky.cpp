#include <stdint.h>
#include <stdbool.h>
#include <cstddef>
#include <stdio.h>

#include "libopencm3/stm32/f4/rcc.h"
#include "libopencm3/stm32/f4/gpio.h"

#define LED_PORT (GPIOA)
#define LED_PIN  (GPIO5)

static void rcc_setup(void) {
  rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
}

static void gpio_setup(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

static void delay_cycles(uint32_t cycles) {
  for (volatile uint32_t i = 0; i < cycles; i++) {
    __asm__("nop");
  }
}

int main(void) {
  rcc_setup();
  gpio_setup();
  bool on = true;
  while (true) {
    if (on) {
      gpio_set(LED_PORT, LED_PIN);
      on = false;
    } else {
      gpio_clear(LED_PORT, LED_PIN);
      on = true;
    }
    delay_cycles(84000000);
  }
  return 0;
}
