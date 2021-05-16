
#include "mqtt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "cJSON.h"
#include "dht11.h"
#include "diversos.h"

#define TAG "MQTT"

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

char *comodo;
char urlComodo[40] = "fse2020/170080366/";

char *obtemMacAddress() {
    uint8_t base_mac_addr[6] = {0};
    char *macAddress = malloc(20);
    esp_efuse_mac_get_default(base_mac_addr);

    snprintf(macAddress, 20,
             "%x:%x:%x:%x:%x:%x", base_mac_addr[0], base_mac_addr[1], base_mac_addr[2], base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);

    return macAddress;
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(conexaoMQTTSemaphore);
            char url[100] = "fse2020/170080366/dispositivos/";
            strcat(url, obtemMacAddress());
            printf("%s", url);
            cJSON* data = NULL;
            data = cJSON_CreateObject();
            cJSON_AddStringToObject(data, "cadastro","novo dispositivo conectado");
            mqtt_envia_mensagem(url,cJSON_Print(data));
            msg_id = esp_mqtt_client_subscribe(client, url, 0);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            trata_resposta(event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{   
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://test.mosquitto.org",
        .port = 1883
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char * topico, char * mensagem)
{
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", message_id);
}

void trata_resposta(char *data){

    cJSON * json = cJSON_Parse(data);

    cJSON *response = cJSON_GetObjectItem(json, "tipo");

    switch (response->valueint)
    { 
    case 0:
        response = cJSON_GetObjectItem(json, "comodo");
        strcat(urlComodo, response->valuestring);
        xTaskCreate(&enviaTemperaturaHumidade,  "Temperatura e humidade", 4096, NULL, 1, NULL);
        break;
    case 1: 
        response = cJSON_GetObjectItem(json, "lampada");
        printf("LAMPADA %d\n",response->valueint);
        comando_lampada(response->valueint);
        break;
    case 2: 
        response = cJSON_GetObjectItem(json, "alarme");
        break;
    default:
        printf("tipo indefinido\n");
        break;
    }   
}

void enviaTemperaturaHumidade()
{
    char *url_temp = calloc(65,sizeof(char));
    char *url_humy= calloc(65,sizeof(char));
    strcat(url_temp, urlComodo);
    strcat(url_temp, "/temperatura");
    strcat(url_humy, urlComodo);
    strcat(url_humy, "/umidade");

    printf("%s\n",url_temp);
    printf("%s\n",url_humy);
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    DHT11_init(GPIO_NUM_4);
    while(true)
    {
      // pega temperatura
      last_read = DHT11_read();

      // preparando json de envio da response
      cJSON* response_temp=NULL, *response_humy= NULL;
      response_temp = cJSON_CreateObject();
      response_humy = cJSON_CreateObject();
      cJSON_AddNumberToObject(response_temp, "temperatura", last_read.temperature);
      cJSON_AddNumberToObject(response_humy, "umidade", last_read.humidity);

      mqtt_envia_mensagem(url_temp, cJSON_Print(response_temp));
      mqtt_envia_mensagem(url_humy, cJSON_Print(response_humy));

      vTaskDelay(2000 / portTICK_PERIOD_MS);
      free(response_temp);
      free(response_humy); 
    }
  }
}