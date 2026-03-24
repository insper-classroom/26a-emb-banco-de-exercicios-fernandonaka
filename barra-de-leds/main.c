#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int BTN_PIN = 22;

const int SW_PIN = 28;

const int LED_PIN_1 = 2;
const int LED_PIN_2 = 3;
const int LED_PIN_3 = 4;
const int LED_PIN_4 = 5;
const int LED_PIN_5 = 6;


volatile int flag_btn = 0;
volatile int sw_mode = 0;


void bar_init(){
    gpio_init(LED_PIN_1);
    gpio_set_dir(LED_PIN_1, GPIO_OUT);
    gpio_init(LED_PIN_2);
    gpio_set_dir(LED_PIN_2, GPIO_OUT);
    gpio_init(LED_PIN_3);
    gpio_set_dir(LED_PIN_3, GPIO_OUT);
    gpio_init(LED_PIN_4);
    gpio_set_dir(LED_PIN_4, GPIO_OUT);
    gpio_init(LED_PIN_5);
    gpio_set_dir(LED_PIN_5, GPIO_OUT);
}

void bar_display(int val){
    if (val < 0) val = 0;
    if (val > 5) val = 5;

    if (val == 0){
        gpio_put(LED_PIN_1,0);
        gpio_put(LED_PIN_2,0);
        gpio_put(LED_PIN_3,0);
        gpio_put(LED_PIN_4,0);
        gpio_put(LED_PIN_5,0);
    } else if (val == 1){
        gpio_put(LED_PIN_1,1);
        gpio_put(LED_PIN_2,0);
        gpio_put(LED_PIN_3,0);
        gpio_put(LED_PIN_4,0);
        gpio_put(LED_PIN_5,0);
    } else if (val == 2){
        gpio_put(LED_PIN_1,1);
        gpio_put(LED_PIN_2,1);
        gpio_put(LED_PIN_3,0);
        gpio_put(LED_PIN_4,0);
        gpio_put(LED_PIN_5,0);
    } else if (val == 3){
        gpio_put(LED_PIN_1,1);
        gpio_put(LED_PIN_2,1);
        gpio_put(LED_PIN_3,1);
        gpio_put(LED_PIN_4,0);
        gpio_put(LED_PIN_5,0);
    }else if (val == 4){
        gpio_put(LED_PIN_1,1);
        gpio_put(LED_PIN_2,1);
        gpio_put(LED_PIN_3,1);
        gpio_put(LED_PIN_4,1);
        gpio_put(LED_PIN_5,0);
    } else if (val == 5){
        gpio_put(LED_PIN_1,1);
        gpio_put(LED_PIN_2,1);
        gpio_put(LED_PIN_3,1);
        gpio_put(LED_PIN_4,1);
        gpio_put(LED_PIN_5,1);
    }

}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge
        if (gpio == BTN_PIN) {
            flag_btn = 1;
        } else if (gpio == SW_PIN){
            sw_mode = 0;
        }
    }
    if (events & GPIO_IRQ_EDGE_RISE) {
        if (gpio == SW_PIN){
            sw_mode = 1;
        }
    }
}

int main() {
    stdio_init_all();
    bar_init();
    int cont = 0;

    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);


    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(SW_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);


    while (true) {
        if (flag_btn){
            if (sw_mode == 0){
                cont += 1;
                bar_display(cont);
            } else {
                cont -= 1;
                bar_display(cont);
            }
            flag_btn = 0;
        }
    }
}
