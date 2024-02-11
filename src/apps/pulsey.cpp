#include <stdint.h>
#include <stdbool.h>
#include <cstddef>
#include <stdio.h>

#include "libopencm3/cm3/systick.h"
#include "libopencm3/cm3/vector.h"
#include "libopencm3/stm32/f4/rcc.h"
#include "libopencm3/stm32/f4/gpio.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/timer.h"

#define CPU_FREQUENCY (84000000)
#define SYSTICK_FREQUENCY (1000)
#define LED_PORT (GPIOA)
#define LED_PIN (GPIO5)
#define WAIT_MS (1)
#define PRESCALER (84)
#define AUTO_RELOAD_REGISTER (1000)
// frequency = CPU_FREQUENCY / ((PRESCALER - 1) * (AUTO_RELOAD_REGISTER - 1))

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
  // gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
  gpio_mode_setup(LED_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, LED_PIN);
  gpio_set_af(LED_PORT, GPIO_AF1, LED_PIN);
}

static void timer_setup(void) {
  rcc_periph_clock_enable(RCC_TIM2);
  timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
  timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
  timer_enable_counter(TIM2);
  timer_enable_oc_output(TIM2, TIM_OC1);
  timer_set_prescaler(TIM2, PRESCALER - 1);
  timer_set_period(TIM2, AUTO_RELOAD_REGISTER - 1);
}

static void timer_pwm_set_duty_cycle(float duty_cycle) {
  // duty_cycle = (ccr / arr) * 100
  // duty_cycle / 100 = ccr / arr
  // (duty_cycle / 100) * arr = ccr
  const float ccr = float (AUTO_RELOAD_REGISTER) * (duty_cycle / 100.0f);
  timer_set_oc_value(TIM2, TIM_OC1, (uint32_t) ccr);
}

int main(void) {
  rcc_setup();
  gpio_setup();
  timer_setup();
  systick_setup();
  uint64_t start_time = get_ticks();
  float duty_cycle = 0.0f;
  timer_pwm_set_duty_cycle(duty_cycle);
  bool up = true;
  float magnitude = 1.0f;
  while (true) {
    if (get_ticks() - start_time >= WAIT_MS) {
      if (up and duty_cycle >= 100.0f) {
        up = false;
        magnitude = -1.0f;
      } else if (!up and duty_cycle <= 0.0f) {
        up = true;
        magnitude = 1.0f;
      }
      duty_cycle += magnitude;
      timer_pwm_set_duty_cycle(duty_cycle);
      // gpio_toggle(LED_PORT, LED_PIN);
      start_time = get_ticks();
    }
  }
  return 0;
}
