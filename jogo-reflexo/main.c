#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"



const int BTN_VERDE = 28;
const int BTN_AMARELO = 26;
const int BTN_VERMELHO = 20;

const int LED_VERDE = 5;
const int LED_AMARELO = 9;
const int LED_VERMELHO = 13;

volatile int flag_fall_verde = 0;
volatile int flag_fall_amarelo = 0;
volatile int flag_fall_vermelho = 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {  // fall edge (events & GPIO_IRQ_EDGE_FALL)
        if (gpio == BTN_VERDE) {
            flag_fall_verde = 1;
        } else if (gpio == BTN_AMARELO){
            flag_fall_amarelo = 1;
        } else if (gpio == BTN_VERMELHO){
            flag_fall_vermelho = 1;
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
    gpio_init(BTN_VERMELHO);
    gpio_set_dir(BTN_VERMELHO, GPIO_IN);
    gpio_pull_up(BTN_VERMELHO);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);
    gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(BTN_VERMELHO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    bool inicio = false;
    int pnts = 0;
    int rodada = 0;
    int ordem[10] = {1,0,2,1,0,1,2,1,0,1};


    while (true) {
        if (!inicio) {
            if (flag_fall_verde) {
                inicio = true;
                rodada = 1;
                pnts = 0;
                flag_fall_verde = 0;
            }
            continue;
        }
        for (int j = 0; j < rodada; j++){
            int cor = ordem[j];

            if (cor == 0){
                gpio_put(LED_VERDE,1);
            } else if (cor == 1){
                gpio_put(LED_AMARELO,1);
            } else {
                gpio_put(LED_VERMELHO,1);              
            }
            sleep_ms(300);  // tempo ligado
            gpio_put(LED_VERDE, 0);
            gpio_put(LED_AMARELO, 0);
            gpio_put(LED_VERMELHO, 0);
            sleep_ms(300); // intervalo 300 ms
        }
        flag_fall_verde = 0;
        flag_fall_amarelo = 0;
        flag_fall_vermelho = 0;        

        bool errou = false;

        for (int j = 0; j < rodada; j++){
            int entrada = -1;
            while (entrada == -1){
                if (flag_fall_verde){
                    entrada = 0;
                    flag_fall_verde = 0;
                } 
                if (flag_fall_amarelo){
                    entrada = 1;
                    flag_fall_amarelo = 0;
                } 
                if (flag_fall_vermelho){
                    entrada = 2;
                    flag_fall_vermelho = 0;
                } 
            }
            if (entrada != ordem[j]){
                errou = true;
                break;
            }
        }

        if (errou) {
            printf("Points %d\n", pnts);
            inicio = false;
            continue;
        }

        pnts = rodada;  
        rodada++;

        if (rodada > 10) {
        printf("Points %d\n", pnts);  
        inicio = false;
        }

    }
}
