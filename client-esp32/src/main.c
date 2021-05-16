#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"

#include "wifi.h"
#include "mqtt.h"
#include "interruption.h"
#include "cJSON.h"

#define LED 2
#define BOTAO 0

static struct dht11_reading last_read;

struct data {
  char * url;
  char * msg;
};

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void *p)
{
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {

    while(true)
    {
      // pega temperatura
      DHT11_init(GPIO_NUM_4);
      last_read = DHT11_read();

      // preparando json de envio da response
      cJSON* response = NULL;
      response = cJSON_CreateObject();
      cJSON_AddNumberToObject(response, "temperatura", last_read.temperature);
      cJSON_AddNumberToObject(response, "umidade", last_read.humidity);

      mqtt_envia_mensagem("fse2020/170080366/dispositivos/1", cJSON_Print(response));

      vTaskDelay(2000 / portTICK_PERIOD_MS);
      //  gpio_pad_select_gpio(BOTAO);
      //  gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
      //   gpio_pad_select_gpio(LED);
      //   gpio_set_direction(LED, GPIO_MODE_OUTPUT);

      //  gpio_set_level(LED, 1);
      //  if(gpio_get_level(BOTAO)==1){
      //   gpio_set_level(LED, 1);
      //  };
      //  gpio_set_level(LED_1, 1);
    }
  }
}

void inicia_mqtt()
{
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    //xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
}

void app_main(void){
  inicia_mqtt();
  // while(1){
  //   printf("...\n");
  //   vTaskDelay(5000 / portTICK_PERIOD_MS);
  // }
  // inicia_mqtt();
  // ativarInerrupcaoAlarme();

  // int temp = 10;

  // while(temp--){
  //   vTaskDelay(2500 / portTICK_PERIOD_MS);
  // }
  // //printf("----------------- Desativando interrupcao\n");

  // desativarInterrupcaoAlarme();
    
}


