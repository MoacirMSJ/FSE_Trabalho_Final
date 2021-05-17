
#include "interruption.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "mqtt.h"

int desativado = 0;

xQueueHandle filaDeInterrupcao;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao(void *params)
{
  char *url = (char*) params;
  int pino;

  while(true)
  {
    if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
    {
      // De-bouncing
      int estado = gpio_get_level(pino);
      if(estado == 1)
      {

        gpio_isr_handler_remove(pino);
        while(gpio_get_level(pino) == estado)
        {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
        if(!desativado){
          printf("Alarme acionado\n");
          cJSON* response_alarme = NULL;
          response_alarme = cJSON_CreateObject();
          cJSON_AddStringToObject(response_alarme,"alarme","acionado");
          mqtt_envia_mensagem(url, cJSON_Print(response_alarme));

          free(response_alarme);
            gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
        }
      }

    }
  }
}

void ativarInerrupcaoAlarme(char *url_alarm){
  desativado = 0;
  gpio_pad_select_gpio(BOTAO_1);

  // Configura o pino do Botão como Entrada
  gpio_set_direction(BOTAO_1, GPIO_MODE_INPUT);

  // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
  gpio_pulldown_en(BOTAO_1);

  // Desabilita o resistor de Pull-up por segurança.
  gpio_pullup_dis(BOTAO_1);

  // Configura pino para interrupção
  gpio_set_intr_type(BOTAO_1, GPIO_INTR_POSEDGE);


  filaDeInterrupcao = xQueueCreate(10, sizeof(int));
  xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, url_alarm, 1, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);

  cJSON* response_alarme = NULL;
  response_alarme = cJSON_CreateObject();
  cJSON_AddStringToObject(response_alarme,"alarme","ativado");
  mqtt_envia_mensagem(url_alarm, cJSON_Print(response_alarme));
  printf("Alarme ativado\n");

  free(response_alarme);
}

void desativarInterrupcaoAlarme(char * url_alarm){

  printf("para alarme.\n");
  gpio_isr_handler_remove(0);
  gpio_intr_disable(0);
  desativado = 1;

  cJSON* response_alarme = NULL;
  response_alarme = cJSON_CreateObject();
  cJSON_AddStringToObject(response_alarme,"alarme","desativado");
  mqtt_envia_mensagem(url_alarm, cJSON_Print(response_alarme));
  free(response_alarme);
  printf("Alarme desativado\n");
}
