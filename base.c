// /**
//  * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
//  *
//  * SPDX-License-Identifier: BSD-3-Clause
//  */

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/gpio.h"


// const int ECHO_PIN = 5;
// const int TRIG_PIN = 3;

// volatile uint32_t start_time = 0;
// volatile uint32_t end_time = 0;
// volatile int flag_echo = 0;
// volatile int flag_timer = 0;
// volatile int flag_falha = 0;

// volatile alarm_id_t alarm = 0;


// void echo_callback(uint gpio, uint32_t events) {
//     if (events & GPIO_IRQ_EDGE_RISE) {
//         start_time = to_us_since_boot(get_absolute_time());
//     } else if (events & GPIO_IRQ_EDGE_FALL) {
//         end_time = to_us_since_boot(get_absolute_time());
//         if (alarm != 0) {
//             cancel_alarm(alarm);
//             alarm = 0;
//         }
//         flag_echo = 1;
//     }
// }

// int64_t alarm_callback(alarm_id_t id, void *user_data) {
//     flag_falha = 1;
//     alarm = 0;
//     return 0; 
// }

// bool timer_callback(repeating_timer_t *rt) {
//     flag_timer = 1;
//     return true; 
// }

// int main() {
//     stdio_init_all();

//     gpio_init(ECHO_PIN);
//     gpio_init(TRIG_PIN);
//     gpio_set_dir(ECHO_PIN, GPIO_IN);
//     gpio_set_dir(TRIG_PIN, GPIO_OUT);
//     gpio_pull_down(ECHO_PIN);

//     gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_callback);

//     repeating_timer_t timer;
//     add_repeating_timer_ms(-3000, timer_callback, NULL, &timer);

//     while (true) {

//         if (flag_timer) {
//             flag_timer = 0;
//             flag_echo = 0;
//             flag_falha = 0;

//             gpio_put(TRIG_PIN, 1);
//             sleep_us(10);
//             gpio_put(TRIG_PIN, 0);

//             alarm = add_alarm_in_ms(50, alarm_callback, NULL, false);
//         }

//         if (flag_falha) {
//             flag_falha = 0;
//             flag_echo = 0;
//             printf("Falha: sensor não respondeu\n");
//         } else if (flag_echo) {
//             flag_echo = 0;

//             uint32_t dt = end_time - start_time;
//             //printf("Duração: %d us\n", dt);
//             float distance = dt * 0.0343 / 2; 
//             printf("Distance: %.2f cm\n", distance);
//         }
//     }
// }
#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


const int BTN_VERDE = 13; 
const int BTN_VERMELHO = 15; 
const int BTN_AZUL = 14;

const int LED_VERDE = 18; 
const int LED_VERMELHO = 16; 
const int LED_AZUL = 17;

volatile int flag_fall_verde = 0;
volatile int flag_fall_vermelho = 0;
volatile int flag_fall_azul = 0;
volatile int flag_rise_azul = 0;
volatile int flag_rise_verde = 0;
volatile int flag_rise_vermelho = 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_VERDE) {
            flag_fall_verde = 1;
        } else if (gpio == BTN_VERMELHO){
            flag_fall_vermelho = 1;
        } else if (gpio == BTN_AZUL){
            flag_fall_azul = 1;
        }
    } else if (events & GPIO_IRQ_EDGE_RISE ) {
        if (gpio == BTN_VERDE) {
            flag_rise_verde = 1;
        } else if (gpio == BTN_VERMELHO){
            flag_rise_vermelho = 1;
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
  gpio_init(BTN_VERMELHO);
  gpio_set_dir(BTN_VERMELHO, GPIO_IN);
  gpio_pull_up(BTN_VERMELHO);
  gpio_init(BTN_AZUL);
  gpio_set_dir(BTN_AZUL, GPIO_IN);
  gpio_pull_up(BTN_AZUL);

  gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
  gpio_set_irq_enabled(BTN_VERMELHO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
  gpio_set_irq_enabled(BTN_AZUL, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

  gpio_init(LED_VERDE);
  gpio_set_dir(LED_VERDE, GPIO_OUT);
  gpio_init(LED_VERMELHO);
  gpio_set_dir(LED_VERMELHO, GPIO_OUT);
  gpio_init(LED_AZUL);
  gpio_set_dir(LED_AZUL, GPIO_OUT);

  int led_state_vermelho = 0;
  int led_state_verde = 0;
  int led_state_azul = 0;

 
  while (true) {
        if (flag_fall_verde) {
            led_state_verde = !led_state_verde;
            gpio_put(LED_VERDE, led_state_verde);
            flag_fall_verde = 0;
        }
        if (flag_rise_verde) {
            gpio_put(LED_VERDE, 0);
            flag_rise_verde = 0;
        }

        if (flag_fall_vermelho) {
            led_state_vermelho = !led_state_vermelho;
            gpio_put(LED_VERMELHO, led_state_vermelho);
            flag_fall_vermelho = 0;
        }
        if (flag_rise_vermelho) {
            gpio_put(LED_VERMELHO, 0);
            flag_rise_vermelho = 0;
        }

        if (flag_fall_azul) {
            led_state_azul = !led_state_azul;
            gpio_put(LED_AZUL, led_state_azul);
            flag_fall_azul = 0;
        }
        if (flag_rise_azul) {
            gpio_put(LED_AZUL, 0);
            flag_rise_azul = 0;
        }
  }
}