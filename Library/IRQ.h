#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

const int BTN_A = 5;

const int BTN_B = 6;

const int BTN_JOY = 22;

void setupBTN(){
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    gpio_init(BTN_JOY);
    gpio_set_dir(BTN_JOY, GPIO_IN);
    gpio_pull_up(BTN_JOY);

}



void resetIRQ(uint gpio, uint32_t events){
    reset_usb_boot(0, 0);
}

