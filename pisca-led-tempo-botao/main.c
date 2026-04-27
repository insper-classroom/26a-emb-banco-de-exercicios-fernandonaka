#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int BTN = 28; // 28;

const int LED = 5; // 9;

volatile int flag_fall = 0;
volatile int flag_rise = 0;

volatile int led_state = 0;

repeating_timer_t timer_led;
volatile int flag_timer = 0;

volatile alarm_id_t alarm = 0;
volatile bool alarm_ativado = false;

volatile uint32_t tempo;
volatile uint32_t delta;


void btn_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN) {
            flag_fall = 1;
            tempo = current_time;        } 
    } else if (events & GPIO_IRQ_EDGE_RISE) {  // rise edge (events & GPIO_IRQ_EDGE_RISE)
        if (gpio == BTN) {
            flag_rise = 1;
            delta = current_time - tempo;
        } 
    }
}

bool timer_callback(repeating_timer_t *rt){
    flag_timer = 1;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_ativado = false;
    flag_timer = 0;
    led_state = 0;
    gpio_put(LED, 0);
    cancel_repeating_timer(&timer_led);
    return 0; 
}

int main() {
    stdio_init_all();

    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if (flag_fall){
            flag_timer = 0;
            led_state = 0;
            gpio_put(LED, 0);
            cancel_repeating_timer(&timer_led);
            if (alarm_ativado){
                cancel_alarm(alarm);
                alarm_ativado = false;
            }
            flag_fall = 0;
        }
        if (flag_rise){
            if (alarm_ativado){
                cancel_alarm(alarm);
                alarm_ativado = false;
            }
            flag_timer = 0;
            add_repeating_timer_ms(200,timer_callback, NULL, &timer_led);
            alarm = add_alarm_in_ms(delta,alarm_callback,NULL,false);
            alarm_ativado = true;
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