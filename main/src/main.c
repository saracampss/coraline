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
#include "button.h"

#define TAG "MAIN"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

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

  xTaskCreate(&wifi_task, "conexao_mqtt", 4096, NULL, 1, NULL);
  xTaskCreate(&comunicacao_servidor_task, "comunicacao_broker", 4096, NULL, 1, NULL);
  xTaskCreate(&oled_display_info_task, "render_display", 2048, NULL, 2, NULL);
  xTaskCreate(&set_plant_status_task, "atualiza_humor_planta", 2048, NULL, 2, NULL);
  xTaskCreate(&dht_task, "dht", 2048, NULL, 3, NULL);
  xTaskCreate(&soil_task, "sensor_solo", 2048, NULL, 3, NULL);
  xTaskCreate(&grava_nvs_task, "armazenamento_nvs", 2048, NULL, 3, NULL);
  xTaskCreate(&button_task, "button_task", 2048, NULL, 1, NULL);
}
