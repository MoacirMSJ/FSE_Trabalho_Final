
#include "interruption.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

/* GPIO Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO18: output
 * GPIO19: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO18 with GPIO4
 * Connect GPIO19 with GPIO5
 * Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5
 *
 */

int desativado = 0;

xQueueHandle filaDeInterrupcao;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao(void *params)
{
  int pino;
  int contador = 0;

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

        contador++;
        printf("Os botões foram acionados %d vezes. Botão: %d\n", contador, pino);
        // Enviar mensagem de alarme ativado usando mqtt
        printf("------------------------------------\n");
        printf("TAO INVADINDO A CASA, CHAMA A PULIÇA\n");
        printf("IIIUUUUUU IIIUUUUUUU IIIUUUUUUUU IIIIIUUUUUU\n");

        // Habilitar novamente a interrupção
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if(!desativado){
            gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
        }
      }

    }
  }
}



void ativarInerrupcaoAlarme(){
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
  xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO_1, gpio_isr_handler, (void *) BOTAO_1);


}

void desativarInterrupcaoAlarme(){
    //disable interrupt
    printf("para alarme.\n");
    gpio_isr_handler_remove(0);
    gpio_intr_disable(0);
    desativado = 1;
    // gpio_intr_disable(0);
    // gpio_intr_disable(0);
    // gpio_intr_disable(0);

}
