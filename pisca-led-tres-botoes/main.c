#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"


const int BTN_VERDE = 18; 
const int BTN_AMARELO = 28; 
const int BTN_AZUL = 22;

const int LED_VERDE = 14; 
const int LED_AMARELO = 6; 
const int LED_AZUL = 10;

volatile int flag_fall_verde = 0;
volatile int flag_fall_amarelo = 0;
volatile int flag_fall_azul = 0;
volatile int flag_rise_azul = 0;
volatile int flag_rise_verde = 0;
volatile int flag_rise_amarelo = 0;
volatile int verde_pressionado = 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_VERDE) {
            verde_pressionado = true;
        } else if (gpio == BTN_AMARELO){
            flag_fall_amarelo = 1;
        } else if (gpio == BTN_AZUL){
            flag_fall_azul = 1;
        }
    } else if (events & GPIO_IRQ_EDGE_RISE ) {
        if (gpio == BTN_VERDE) {
            verde_pressionado = false;
        } else if (gpio == BTN_AMARELO){
            flag_rise_amarelo = 1;
        } else if (gpio == BTN_AZUL){
            flag_rise_azul = 1;
        }
    }
}

int main() {
  stdio_init_all();

  gpio_init(BTN_VERDE);
  gpio_set_dir(BTN_VERDE, GPIO_IN);
  gpio_pull_up(BTN_VERDE);
  gpio_init(BTN_AMARELO);
  gpio_set_dir(BTN_AMARELO, GPIO_IN);
  gpio_pull_up(BTN_AMARELO);
  gpio_init(BTN_AZUL);
  gpio_set_dir(BTN_AZUL, GPIO_IN);
  gpio_pull_up(BTN_AZUL);

  gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
  gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
  gpio_set_irq_enabled(BTN_AZUL, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

  gpio_init(LED_VERDE);
  gpio_set_dir(LED_VERDE, GPIO_OUT);
  gpio_init(LED_AMARELO);
  gpio_set_dir(LED_AMARELO, GPIO_OUT);
  gpio_init(LED_AZUL);
  gpio_set_dir(LED_AZUL, GPIO_OUT);

  int led_state_amarelo = 0;
  int led_state_verde = 0;
  int led_state_azul = 0;

  bool amarelo = false;
  bool azul = false;
 
  while (true) {
    if (flag_fall_amarelo){
      amarelo = !amarelo;
      flag_fall_amarelo = 0;
    } 
    if (amarelo == true){
      led_state_amarelo = !led_state_amarelo;
      gpio_put(LED_AMARELO,led_state_amarelo);
    } else {
      gpio_put(LED_AMARELO,0);
    }
    if (flag_rise_azul){
      azul = !azul;
      flag_rise_azul = 0;
    }
    if (azul == true){
      led_state_azul = !led_state_azul;
      gpio_put(LED_AZUL,led_state_azul);
    } else {
      gpio_put(LED_AZUL,0);
    }

    if (verde_pressionado){
      led_state_verde = !led_state_verde;
      gpio_put(LED_VERDE,led_state_verde);
    } else {
      gpio_put(LED_VERDE,0);
    }

    sleep_ms(200);
  }
}