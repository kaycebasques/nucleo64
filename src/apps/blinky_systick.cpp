#include <stdint.h>
#include <stdbool.h>
#include <cstddef>
#include <stdio.h>

#include "libopencm3/cm3/systick.h"
#include "libopencm3/cm3/vector.h"
#include "libopencm3/stm32/f4/rcc.h"
#include "libopencm3/stm32/f4/gpio.h"

#define CPU_FREQUENCY (84000000)
#define SYSTICK_FREQUENCY (1000)
#define LED_PORT (GPIOA)
#define LED_PIN (GPIO5)
#define WAIT_MS (5000)

volatile uint64_t ticks = 0;

// Our convention is to style `systick` as 1 word but this needs to match the
// function name defined in libopencm3
void sys_tick_handler(void) {
  ticks += 1;
}

static uint64_t get_ticks(void) {
  return ticks;
}

static void systick_setup(void) {
  systick_set_frequency(SYSTICK_FREQUENCY, CPU_FREQUENCY);
  systick_counter_enable();
  systick_interrupt_enable();
}

static void rcc_setup(void) {
  rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);
}

static void gpio_setup(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

int main(void) {
  rcc_setup();
  gpio_setup();
  systick_setup();
  uint64_t start_time = get_ticks();
  while (true) {
    if (get_ticks() - start_time >= WAIT_MS) {
      gpio_toggle(LED_PORT, LED_PIN);
      start_time = get_ticks();
    }
  }
  return 0;
}
