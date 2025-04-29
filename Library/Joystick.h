#include "hardware/adc.h"
#include "pico/stdlib.h"

/**
 * @brief pinagem dos GPIO 
 */

const int ADC_1 = 26;
const int ADC_2 = 27;

const int X = 1;
const int Y = 0;

void setupJoy(){

    adc_init();

    //Configuração dos Pinos como entradas ADC(alta impedância, dificultando a passagem elétrica)
    //sem resistores pull_up
    adc_gpio_init(ADC_1);
    adc_gpio_init(ADC_2);


}

int read_adc_x(){
    adc_select_input(X);
    uint adc_x_raw = adc_read();

    return adc_x_raw;
}

uint read_adc_y(){
    adc_select_input(Y);
    uint adc_y_raw = adc_read();

    return adc_y_raw;
}