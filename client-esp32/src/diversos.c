#include "diversos.h"

void comando_lampada(int intensidade){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    if(intensidade == 1){
        gpio_set_level(LED, 1);
    }else
    {
        gpio_set_level(LED, 0);
    }
}