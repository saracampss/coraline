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
#include "esp_adc/adc_oneshot.h"

#include "wifi.h"
#include "gpio_setup.h"
#include "soil_moisture.h"
#include "http_client.h"
#include "mqtt.h"
#include "oled.h"
#include "dht22.h"
#include "nvs.h"

#define TAG "MAIN"

#define MOISTURE ADC_CHANNEL_6

extern int PLANT_STATUS;
extern int DISPLAY_MODE;

float TEMPERATURE = 0.0;
float HUMIDITY = 0.0;
float SOIL_MOISTURE = 0.0;

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void soil_task(void *params)
{
  adc_init(ADC_UNIT_1);

  pinMode(MOISTURE, GPIO_ANALOG);

  while (true)
  {
    int moisture = analogRead(MOISTURE);

    SOIL_MOISTURE = 100.0 - ((moisture / 4095.0) * 100.0);

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void dht_task(void *params)
{
  set_dht_gpio(4);

  while (1)
  {
    int ret = read_dht();

    error_handler(ret);

    HUMIDITY = get_humidity();
    TEMPERATURE = get_temperature();

    vTaskDelay(3000 / portTICK_PERIOD_MS);
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
  char mensagem[200];
  char jsonAtributos[200];

  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      sprintf(mensagem, "{\"moisture\": %f, \"temperature\": %f, \"humidity\": %f, \n\"plant_status\": %d}", SOIL_MOISTURE, TEMPERATURE, HUMIDITY, PLANT_STATUS);
      mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

      sprintf(jsonAtributos, "{\"display_mode\": %d, \n\"plant_status\": %d}", DISPLAY_MODE, PLANT_STATUS);
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

  inicia_valores_nvs();

  conexaoWifiSemaphore = xSemaphoreCreateBinary();
  conexaoMQTTSemaphore = xSemaphoreCreateBinary();

  wifi_start();
  oled_start();
  set_dht_gpio(4);

  xTaskCreate(&conectado_wifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trata_comunicacao_servidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
  xTaskCreate(&oled_display_info_task, "Atualização do Display", 2048, NULL, 2, NULL);
  xTaskCreate(&set_plant_status_task, "Atualização do Humor da Planta", 2048, NULL, 2, NULL);
  xTaskCreate(&dht_task, "Conexão com sensor DHT22", 2048, NULL, 3, NULL);
  xTaskCreate(&soil_task, "Conexão com sensor de umidade de solo", 2048, NULL, 3, NULL);
  xTaskCreate(&grava_nvs_task, "Armazena dados de estado no NVS", 2048, NULL, 3, NULL);
}
