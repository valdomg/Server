#include <stdio.h>
#include "pico/stdlib.h"

#include "Library/IRQ.h"


int main()
{
    stdio_init_all();

    setupBTN();
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_RISE, true, &resetIRQ);

    

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
