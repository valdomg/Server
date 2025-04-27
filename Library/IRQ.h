#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

const int BTN_A = 5;

void setupBTN(){
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

}

void resetIRQ(uint gpio, uint32_t events){
    reset_usb_boot(0, 0);
}

