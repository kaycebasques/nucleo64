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
#define WAIT_MS (100)

// systick app pulled in .../f4/rcc.h - not needed?
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/usart.h"
#include "libopencm3/cm3/nvic.h"

static void uart_setup(void) {
  rcc_periph_clock_enable(RCC_USART2);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_set_databits(USART2, 8);
  usart_set_baudrate(USART2, 115200);
  usart_set_parity(USART2, 0);
  usart_set_stopbits(USART2, 1);
  usart_enable_rx_interrupt(USART2);
  nvic_enable_irq(NVIC_USART2_IRQ);
  usart_enable(USART2);
}

// static void uart_write(uint8_t* data, const uint32_t length) {
//   for (uint32_t i = 0; i < length; i++) {
//     uart_write_byte(data[i]);
//   }
// }

static void uart_write_byte(uint8_t data) {
  usart_send_blocking(USART2, (uint16_t) data);
}

static uint8_t data_buffer = 0U;
static bool data_available = false;

void usart2_isr(void) {
  const bool overrun_occurred = usart_get_flag(USART2, USART_FLAG_ORE) == 1;
  const bool received_data = usart_get_flag(USART2, USART_FLAG_RXNE) == 1;
  if (received_data || overrun_occurred) {
    data_buffer = (uint8_t) usart_recv(USART2);
    data_available = true;
  }
}

uint32_t uart_read(uint8_t* data, const uint32_t length) {
  if (length > 0 && data_available) {
    *data = data_buffer;
    data_available = false;
    return 1;
  }
  return 0;
}

uint8_t uart_read_byte(void) {
  data_available = false;
  return data_buffer;
}

bool uart_data_available(void) {
  return data_available;
}

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

#define UART_PORT (GPIOA)
#define RX_PIN (GPIO3)
#define TX_PIN (GPIO2)

static void gpio_setup(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
  gpio_mode_setup(UART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, TX_PIN | RX_PIN);
  gpio_set_af(UART_PORT, GPIO_AF7, TX_PIN | RX_PIN);
}

int main(void) {
  rcc_setup();
  gpio_setup();
  systick_setup();
  uart_setup();
  uint64_t start_time = get_ticks();
  while (true) {
    if (get_ticks() - start_time >= WAIT_MS) {
      gpio_toggle(LED_PORT, LED_PIN);
      start_time = get_ticks();
    }
    if (uart_data_available()) {
      uint8_t data = uart_read_byte();
      uart_write_byte(data + 1);
    }
  }
  return 0;
}
