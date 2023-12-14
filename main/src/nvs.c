#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#define TAG "NVS"

extern int DISPLAY_MODE;
extern int PLANT_STATUS;
extern float SOIL_MOISTURE;
extern float TEMPERATURE;
extern float HUMIDITY;

int le_valor_nvs(char *chave)
{
    // Inicia o acesso à partição padrão nvs
    ESP_ERROR_CHECK(nvs_flash_init());

    // Inicia o acesso à partição personalizada
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    int32_t valor = 0;
    nvs_handle particao_padrao_handle;

    // Abre o acesso à partição nvs
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);

    // Abre o acesso à partição DadosNVS
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READONLY, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(TAG, "Namespace: armazenamento, não encontrado");
    }
    else
    {
        esp_err_t res = nvs_get_i32(particao_padrao_handle, chave, &valor);

        switch (res)
        {
        case ESP_OK:
            // printf("Valor armazenado: %d\n", (int) valor);
            break;
        case ESP_ERR_NOT_FOUND:
            ESP_LOGE(TAG, "Valor não encontrado");
            return -1;
        default:
            ESP_LOGE(TAG, "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
            return -1;
            break;
        }

        nvs_close(particao_padrao_handle);
    }

    return valor;
}

void grava_valor_nvs(int32_t valor, char *chave)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    // ESP_ERROR_CHECK(nvs_flash_init_partition("DadosNVS"));

    nvs_handle particao_padrao_handle;

    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);
    // esp_err_t res_nvs = nvs_open_from_partition("DadosNVS", "armazenamento", NVS_READWRITE, &particao_padrao_handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE(TAG, "Namespace armazenamento, não encontrado");
    }

    esp_err_t res = nvs_set_i32(particao_padrao_handle, chave, valor);

    if (res != ESP_OK)
    {
        ESP_LOGE(TAG, "Não foi possível escrever [%s]", esp_err_to_name(res));
    }

    ESP_LOGI(TAG, "Valor escrito [%s]", chave);

    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);
}

void grava_nvs_task()
{
    while (true)
    {
        grava_valor_nvs(DISPLAY_MODE, "display_mode");
        grava_valor_nvs(PLANT_STATUS, "plant_status");
        grava_valor_nvs(SOIL_MOISTURE, "soil_moisture");
        grava_valor_nvs(TEMPERATURE, "temperature");
        grava_valor_nvs(HUMIDITY, "humidity");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void inicia_valores_nvs()
{
    int display_mode_nvs = le_valor_nvs("display_mode");
    int plant_status_nvs = le_valor_nvs("plant_status");
    float soil_moisture_nvs = le_valor_nvs("soil_moisture");
    float temperature_nvs = le_valor_nvs("temperature");
    float humidity_nvs = le_valor_nvs("humidity");

    ESP_LOGI(TAG, "Valor armazenado MODO DO DISPLAY: %d", display_mode_nvs);
    ESP_LOGI(TAG, "Valor armazenado HUMOR DA CORALINE: %d", plant_status_nvs);
    ESP_LOGI(TAG, "Valor armazenado UMIDADE SOLO: %.1f", soil_moisture_nvs);
    ESP_LOGI(TAG, "Valor armazenado TEMPERATURA: %.1f", temperature_nvs);
    ESP_LOGI(TAG, "Valor armazenado UMIDADE AR: %.1f", humidity_nvs);

    DISPLAY_MODE = display_mode_nvs;
    PLANT_STATUS = plant_status_nvs;
    SOIL_MOISTURE = soil_moisture_nvs;
    TEMPERATURE = temperature_nvs;
    HUMIDITY = humidity_nvs;
}
