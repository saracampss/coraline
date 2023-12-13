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

#include "wifi.h"
#include "http_client.h"
#include "mqtt.h"
#include "oled.h"

int PLANT_STATUS = 0;

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void oled_task(void *params)
{
  while (true)
  {
    oled_loop();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void conectado_wifi(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      mqtt_start();
    }
  }
}

void trata_comunicacao_servidor(void *params)
{
  char mensagem[50];
  char jsonAtributos[200];

  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      float temperatura = 20.0 + (float)rand() / (float)(RAND_MAX / 10.0);
      sprintf(mensagem, "{\"rand_temp\": %f,\n\"humor_status\": %d}", temperatura, PLANT_STATUS);
      mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

      sprintf(jsonAtributos, "{\"humor_status\": %d}", PLANT_STATUS);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();

  wifi_start();
  oled_start();

  xTaskCreate(&conectado_wifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trata_comunicacao_servidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
  xTaskCreate(&oled_task, "Atualização do Display", 2048, NULL, 2, NULL);
}
