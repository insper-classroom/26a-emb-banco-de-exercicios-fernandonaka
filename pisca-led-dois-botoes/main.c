#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_VERDE = 28;
const int BTN_AMARELO = 26;

const int LED_VERDE = 5;
const int LED_AMARELO = 9;

volatile int flag_fall_verde = 0;
volatile int flag_fall_amarelo = 0;


volatile int led_state_verde = 0;
volatile int led_state_amarelo = 0;

repeating_timer_t timer_led_verde;
repeating_timer_t timer_led_amarelo;
volatile int flag_timer_verde = 0;
volatile int flag_timer_amarelo = 0;

volatile alarm_id_t alarm_verde = 0;
volatile alarm_id_t alarm_amarelo = 0;

volatile bool alarm_ativado_verde = true;
volatile bool alarm_ativado_amarelo = true;



void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_VERDE) {
            flag_fall_verde = 1;
        } else if (gpio == BTN_AMARELO){
            flag_fall_amarelo = 1;
        } 
    }
}

bool timer_callback_verde(repeating_timer_t *rt){
    flag_timer_verde = 1;
    return true;
}
bool timer_callback_amarelo(repeating_timer_t *rt){
    flag_timer_amarelo = 1;
    return true;
}

int64_t alarm_callback_verde(alarm_id_t id, void *user_data) {
    alarm_ativado_verde = true;
    flag_timer_verde = 0;
    gpio_put(LED_VERDE, 0);
    led_state_verde = 0;
    cancel_repeating_timer(&timer_led_verde);
    if (!alarm_ativado_amarelo){
        alarm_ativado_amarelo = true;
        flag_timer_amarelo = 0;
        gpio_put(LED_AMARELO, 0);
        led_state_amarelo = 0;
        cancel_repeating_timer(&timer_led_amarelo);
        cancel_alarm(alarm_amarelo);
        alarm_amarelo = 0;
    }
    alarm_verde = 0;
    return 0; 
}
int64_t alarm_callback_amarelo(alarm_id_t id, void *user_data) {
    alarm_ativado_amarelo = true;
    flag_timer_amarelo = 0;
    gpio_put(LED_AMARELO, 0);
    led_state_amarelo = 0;
    cancel_repeating_timer(&timer_led_amarelo);
    if (!alarm_ativado_verde){
        alarm_ativado_verde = true;
        flag_timer_verde = 0;
        gpio_put(LED_VERDE, 0);
        led_state_verde = 0;
        cancel_repeating_timer(&timer_led_verde);
        cancel_alarm(alarm_verde);
        alarm_verde = 0;
    }
    alarm_amarelo = 0;
    return 0; 
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


    gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);

    //int cor;



    while (true) {
        if (flag_fall_verde){
            flag_fall_verde = 0;
            //cor = 0;
            if (alarm_ativado_verde){
                alarm_ativado_verde = false;
                led_state_verde = 1;
                gpio_put(LED_VERDE, led_state_verde);
                alarm_verde = add_alarm_in_ms(1000,alarm_callback_verde,NULL,false);
                add_repeating_timer_ms(200,timer_callback_verde, NULL, &timer_led_verde);        
            } 
        }
        if (flag_fall_amarelo){
            flag_fall_amarelo = 0;
            //cor = 1;
            if (alarm_ativado_amarelo){
                alarm_ativado_amarelo = false;
                led_state_amarelo= 1;
                gpio_put(LED_AMARELO, led_state_amarelo);
                alarm_amarelo = add_alarm_in_ms(2000,alarm_callback_amarelo,NULL,false);
                add_repeating_timer_ms(500,timer_callback_amarelo, NULL, &timer_led_amarelo);
            } 
        }
        if (flag_timer_verde){
            flag_timer_verde = 0;
            if (!alarm_ativado_verde) {
                led_state_verde = !led_state_verde;
                gpio_put(LED_VERDE, led_state_verde);
            }
        }
        if (flag_timer_amarelo){
            flag_timer_amarelo = 0;
            if (!alarm_ativado_amarelo) {
                led_state_amarelo = !led_state_amarelo;
                gpio_put(LED_AMARELO, led_state_amarelo);
            }
        }



    }

    return 0;
}