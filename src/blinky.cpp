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
  // A key insight for me from Low Byte Episode 1: compilers are smart and can
  // optimize away your spin logic. Marking `i` as `volatile` prevents that.
  // https://youtu.be/06ICtJjPKlA?t=1912
  for (volatile uint32_t i = 0; i < cycles; i++) {
    __asm__("nop");
  }
}

int main(void) {
  rcc_setup();
  gpio_setup();
  while (true) {
    gpio_toggle(LED_PORT, LED_PIN);
    // Low Byte Episode 1 says that 1 second should roughly equal:
    //   84,000,000 / 3 = 28,000,000
    // The RCC clock is set to 84MHz and the delay loop runs
    // about 3 instructions: the comparison of `i` to `cycles`, the
    // incrementing of `i`, and the no-op instruction. That calculation doesn't
    // work in my environment. The number below is closer to a 1-second-on then
    // 1-second-off loop. I guess that means `delay_cycles()` compiles to
    // 10 instructions.
    delay_cycles(84000000 / 10);
  }
  return 0;
}
