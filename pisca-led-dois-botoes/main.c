#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_VERDE = 28; // 28;
const int BTN_AMARELO = 26; // 26;

const int LED_VERDE = 5; // 5;
const int LED_AMARELO = 9; // 9;

volatile int flag_fall_verde = 0;
volatile int flag_fall_amarelo = 0;

repeating_timer_t timer_led_verde;
repeating_timer_t timer_led_amarelo;
volatile int flag_timer_verde = 0;
volatile int flag_timer_amarelo = 0;


volatile alarm_id_t alarm_verde = 0;
volatile alarm_id_t alarm_amarelo = 0;
volatile bool alarm_ativado_verde = false;
volatile bool alarm_ativado_amarelo = false;


volatile int led_state_verde = 0;
volatile int led_state_amarelo = 0;

bool timer_callback_verde(repeating_timer_t *rt){
    flag_timer_verde = 1;
    return true;
}
bool timer_callback_amarelo(repeating_timer_t *rt){
    flag_timer_amarelo = 1;
    return true;
}

int64_t alarm_callback_verde(alarm_id_t id, void *user_data) {
    alarm_ativado_verde = false;
    cancel_repeating_timer(&timer_led_verde);
    led_state_verde = 0;
    gpio_put(LED_VERDE, led_state_verde);

    if (alarm_ativado_amarelo){
        alarm_ativado_amarelo = false;
        cancel_repeating_timer(&timer_led_amarelo);
        led_state_amarelo = 0;
        gpio_put(LED_AMARELO, led_state_amarelo);
    }
    return 0; 
}

int64_t alarm_callback_amarelo(alarm_id_t id, void *user_data) {
    alarm_ativado_amarelo = false;
    cancel_repeating_timer(&timer_led_amarelo);
    led_state_amarelo = 0;
    gpio_put(LED_AMARELO, led_state_amarelo);

    if (alarm_ativado_verde){
        alarm_ativado_verde = false;
        cancel_repeating_timer(&timer_led_verde);
        led_state_verde = 0;
        gpio_put(LED_VERDE, led_state_verde);
    }
    return 0; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_VERDE) {
            flag_fall_verde = 1;
        } else if (gpio == BTN_AMARELO){
            flag_fall_amarelo = 1;
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
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL, true);

    int led_state_verde = 0;
    int led_state_amarelo = 0; 

    while (true) {
        if (flag_fall_verde){
            if (!alarm_ativado_verde){
                alarm_ativado_verde = true;
                alarm_verde = add_alarm_in_ms(1000,alarm_callback_verde,NULL,false);
                add_repeating_timer_ms(200,timer_callback_verde, NULL, &timer_led_verde);
            }
            flag_fall_verde = 0;
        }
        if (flag_fall_amarelo){
            if (!alarm_ativado_amarelo){
                alarm_ativado_amarelo = true;
                alarm_amarelo = add_alarm_in_ms(2000,alarm_callback_amarelo,NULL,false);
                add_repeating_timer_ms(500,timer_callback_amarelo, NULL, &timer_led_amarelo);
            }
            flag_fall_amarelo = 0;
        }

        if (flag_timer_verde){
            flag_timer_verde = 0;
            led_state_verde = !led_state_verde;
            gpio_put(LED_VERDE, led_state_verde);
        }
        if (flag_timer_amarelo){
            flag_timer_amarelo = 0;
            led_state_amarelo = !led_state_amarelo;
            gpio_put(LED_AMARELO, led_state_amarelo);
        }
    }
}

