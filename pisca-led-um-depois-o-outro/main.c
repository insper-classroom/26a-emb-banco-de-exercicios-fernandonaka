#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"


// lembrar da relação: T = 1/f
// ex: f = 5hz ---> T = 1/5 = 0,2 ---> toggle de 100ms (metade)

const int BTN_AZUL = 19;
const int BTN_AMARELO = 26;

const int LED_AZUL = 14;
const int LED_AMARELO = 10;

volatile int flag_fall_azul = 0;
volatile int flag_fall_amarelo = 0;

repeating_timer_t timer_led_azul;
repeating_timer_t timer_led_amarelo;
volatile int flag_timer_azul = 0;
volatile int flag_timer_amarelo = 0;

volatile alarm_id_t alarm_azul = 0;
volatile alarm_id_t alarm_amarelo = 0;


volatile bool alarm_ativado_azul = false;
volatile bool alarm_ativado_amarelo = false;

volatile bool botao_amarelo = false;
volatile bool botao_azul = false;

int64_t alarm_callback_azul(alarm_id_t id, void *user_data);
int64_t alarm_callback_amarelo(alarm_id_t id, void *user_data);


void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_AZUL) {
            flag_fall_azul = 1;
        } else if (gpio == BTN_AMARELO){
            flag_fall_amarelo = 1;
        }
    }
}

bool timer_callback_azul(repeating_timer_t *rt){
    flag_timer_azul = 1;
    return true;
}
bool timer_callback_amarelo(repeating_timer_t *rt){
    flag_timer_amarelo = 1;
    return true;
}

int64_t alarm_callback_azul(alarm_id_t id, void *user_data) {
    alarm_ativado_azul = false;
    cancel_repeating_timer(&timer_led_azul);
    gpio_put(LED_AZUL, 0);
    if (botao_amarelo){
        alarm_ativado_amarelo = true;
        add_repeating_timer_ms(100,timer_callback_amarelo, NULL, &timer_led_amarelo);
        alarm_amarelo = add_alarm_in_ms(1000,alarm_callback_amarelo,NULL,false);
        botao_amarelo = false;
    }
    return 0; 
}

int64_t alarm_callback_amarelo(alarm_id_t id, void *user_data) {
    alarm_ativado_amarelo = false;
    cancel_repeating_timer(&timer_led_amarelo);
    gpio_put(LED_AMARELO, 0);
    if (botao_azul){
        alarm_ativado_azul = true;
        add_repeating_timer_ms(250,timer_callback_azul, NULL, &timer_led_azul);
        alarm_azul = add_alarm_in_ms(2000,alarm_callback_azul,NULL,false);
        botao_azul = false;
    }
    return 0; 
}

int main() {
    stdio_init_all();
    gpio_init(BTN_AZUL);
    gpio_set_dir(BTN_AZUL, GPIO_IN);
    gpio_pull_up(BTN_AZUL);
    gpio_init(BTN_AMARELO);
    gpio_set_dir(BTN_AMARELO, GPIO_IN);
    gpio_pull_up(BTN_AMARELO);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN_AZUL, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL, true);

    int led_state_azul = 0;
    int led_state_amarelo = 0; 

    while (true) {
        if (flag_fall_azul){
            if (!alarm_ativado_azul && !alarm_ativado_amarelo){
                alarm_ativado_azul = true;
                botao_amarelo = true;
                alarm_azul = add_alarm_in_ms(2000,alarm_callback_azul,NULL,false);
                add_repeating_timer_ms(250,timer_callback_azul, NULL, &timer_led_azul);
            }
            flag_fall_azul = 0;
        }
        if (flag_fall_amarelo){
            if (!alarm_ativado_amarelo && !alarm_ativado_azul){
                alarm_ativado_amarelo = true;
                botao_azul = true;
                alarm_amarelo = add_alarm_in_ms(1000,alarm_callback_amarelo,NULL,false);
                add_repeating_timer_ms(100,timer_callback_amarelo, NULL, &timer_led_amarelo);
            }
            flag_fall_amarelo = 0;
        }

        if (flag_timer_azul){
            flag_timer_azul = 0;
            led_state_azul = !led_state_azul;
            gpio_put(LED_AZUL, led_state_azul);
        }
        if (flag_timer_amarelo){
            flag_timer_amarelo = 0;
            led_state_amarelo = !led_state_amarelo;
            gpio_put(LED_AMARELO, led_state_amarelo);
        }
    }
}

