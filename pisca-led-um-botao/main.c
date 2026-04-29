#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN = 28;

const int LED_AMARELO = 5;
const int LED_AZUL = 9;

volatile int flag_fall = 0;

repeating_timer_t timer_led_amarelo;
repeating_timer_t timer_led_azul;
volatile int flag_timer_amarelo = 0;
volatile int flag_timer_azul = 0;

volatile alarm_id_t alarm = 0;
volatile bool alarm_ativado = false;

volatile int led_state_amarelo = 0;
volatile int led_state_azul = 0;

bool timer_callback_amarelo(repeating_timer_t *rt){
    flag_timer_amarelo = 1;
    return true;
}
bool timer_callback_azul(repeating_timer_t *rt){
    flag_timer_azul = 1;
    return true;
}

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    alarm_ativado = false;
    cancel_repeating_timer(&timer_led_amarelo);
    led_state_amarelo = 0;
    gpio_put(LED_AMARELO, led_state_amarelo);

    cancel_repeating_timer(&timer_led_azul);
    led_state_azul = 0;
    gpio_put(LED_AZUL, led_state_azul);
    return 0; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN) {
            flag_fall = 1;
        } 
    }
}

int main() {
    stdio_init_all();
    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {

        if (flag_fall){
            if (!alarm_ativado){
                alarm_ativado = true;
                alarm = add_alarm_in_ms(5000,alarm_callback,NULL,false);
                add_repeating_timer_ms(500,timer_callback_amarelo, NULL, &timer_led_amarelo);
                add_repeating_timer_ms(150,timer_callback_azul, NULL, &timer_led_azul);
            }
            flag_fall = 0;
        }

        if (flag_timer_amarelo){
            flag_timer_amarelo = 0;
            led_state_amarelo = !led_state_amarelo;
            gpio_put(LED_AMARELO, led_state_amarelo);
        }
        if (flag_timer_azul){
            flag_timer_azul = 0;
            led_state_azul = !led_state_azul;
            gpio_put(LED_AZUL, led_state_azul);
        }
    }
    return 0;
}

