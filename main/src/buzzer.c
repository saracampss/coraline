#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "gpio_setup.h"

#define TAG = "BUZZER";
#define BUZZER_PIN 13

extern int PLANT_STATUS;

void play_buzzer()
{
    pinMode(BUZZER_PIN, GPIO_OUTPUT);
    TickType_t startTime = xTaskGetTickCount();

    while (PLANT_STATUS == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(13, 1);

            vTaskDelay(75 / portTICK_PERIOD_MS);

            digitalWrite(13, 0);

            vTaskDelay(75 / portTICK_PERIOD_MS);
        }

        vTaskDelay(235 / portTICK_PERIOD_MS);

        for (int i = 0; i < 5; i++)
        {
            digitalWrite(13, 1);

            vTaskDelay(75 / portTICK_PERIOD_MS);

            digitalWrite(13, 0);

            vTaskDelay(75 / portTICK_PERIOD_MS);
        }

        vTaskDelay(235 / portTICK_PERIOD_MS);

        TickType_t elapsedTime = xTaskGetTickCount() - startTime;

        if (elapsedTime >= pdMS_TO_TICKS(10000)) // 10 segundos
            break;
    }
}
