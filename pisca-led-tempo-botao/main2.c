/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int BTN = 28;
const int LED = 5;
volatile int led_state = 0;


volatile int flag_fall = 0;
volatile int flag_rise = 0;

volatile uint32_t start_time = 0;
volatile uint32_t end_time = 0;

volatile alarm_id_t alarm = 0;
volatile int flag_timer = 0;
repeating_timer_t timer;



void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN) {
            start_time = to_us_since_boot(get_absolute_time());
            flag_fall = 1;
        }
    } else if (events & GPIO_IRQ_EDGE_RISE){
        if (gpio == BTN){
            end_time = to_us_since_boot(get_absolute_time());
            flag_rise = 1;
        }
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED, 0);
    alarm = 0;
    cancel_repeating_timer(&timer);
    return 0; 
}

bool timer_callback(repeating_timer_t *rt) {
    flag_timer = 1;
    return true; 
}

int main() {
    stdio_init_all();
    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {
        if (flag_fall){
            cancel_repeating_timer(&timer);
            if (alarm) {
                cancel_alarm(alarm);
                alarm = 0;
            }
            flag_timer = 0;
            led_state = 0;
            gpio_put(LED, 0);
            flag_fall = 0;
        }
        if (flag_rise){
            uint32_t dt = end_time - start_time;
            cancel_repeating_timer(&timer);
            if (alarm) {
                cancel_alarm(alarm);
                alarm = 0;
            }
            alarm = add_alarm_in_us(dt, alarm_callback, NULL, false);
            add_repeating_timer_ms(200, timer_callback, NULL, &timer);
            flag_rise = 0;
        }
        if (flag_timer){
            led_state = !led_state;
            gpio_put(LED, led_state);
            flag_timer = 0;
        }
    }
    return 0;
}