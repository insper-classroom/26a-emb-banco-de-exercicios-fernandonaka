
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

const int BTN = 28;

const int LED_VERDE = 5;
const int LED_AZUL = 9;

volatile int flag_fall = 0;

repeating_timer_t timer_led_verde;
repeating_timer_t timer_led_azul;
volatile int flag_timer_verde = 0;
volatile int flag_timer_azul = 0;

volatile alarm_id_t alarm = 0;

volatile bool alarm_ativado= false;

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN) {
            flag_fall = 1;
        }
    }
}

bool timer_callback_verde(repeating_timer_t *rt){
    flag_timer_verde = 1;
    return true;
}
bool timer_callback_azul(repeating_timer_t *rt){
    flag_timer_azul = 1;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_ativado = false;
    cancel_repeating_timer(&timer_led_verde);
    cancel_repeating_timer(&timer_led_azul);
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);

    return 0; 
}

int main() {
    stdio_init_all();
    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    int led_state_verde = 0;
    int led_state_azul = 0; 

    while (true) {
        if (flag_fall){
            if (!alarm_ativado){
                alarm_ativado = true;
                alarm = add_alarm_in_ms(5000,alarm_callback,NULL,false);
                add_repeating_timer_ms(500,timer_callback_verde, NULL, &timer_led_verde);        
                add_repeating_timer_ms(100,timer_callback_azul, NULL, &timer_led_azul);        
            }
            flag_fall = 0;
        }
        if (flag_timer_verde){
            flag_timer_verde = 0;
            led_state_verde = !led_state_verde;
            gpio_put(LED_VERDE, led_state_verde);
        }
        if (flag_timer_azul){
            flag_timer_azul = 0;
            led_state_azul = !led_state_azul;
            gpio_put(LED_AZUL, led_state_azul);
        }
    }
}
