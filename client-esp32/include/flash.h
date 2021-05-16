#ifndef FLASH_H
#define FLASH_H

#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define STORAGE_NAMESPACE "storage"

typedef struct {
    char *comodo;
    int32_t alarme;
}InformacoesNVS;

int32_t salvarInfoComodoNVS(char *nomeComodo);
int32_t salvarInfoAlameNVS(int32_t alarme);
InformacoesNVS informacoesCadastradasNVS();
void inicializaNVS();

#endif