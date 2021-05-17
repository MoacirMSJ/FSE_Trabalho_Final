#ifndef INTERRUPTION_H
#define INTERRUPTION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/gpio.h"

#define BOTAO_1 0
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

gpio_config_t io_conf;


void ativarInerrupcaoAlarme(char * url);
void desativarInterrupcaoAlarme(char * url);

//https://github.com/espressif/esp-idf/blob/639e7ad494d93fc82159f9fe854041bc43a96d5f/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c
#endif