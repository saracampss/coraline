#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "gpio_setup.h"

#define TAG = "BUZZER";

// Definindo o pino do buzzer
#define BUZZER_PIN 13

// Definindo a configuração do canal PWM para o buzzer
#define BUZZER_PWM_CHANNEL LEDC_CHANNEL_0
#define BUZZER_PWM_TIMER LEDC_TIMER_0

extern int PLANT_STATUS;
int IS_PLAYING = 0;

void play_buzzer()
{
    if (IS_PLAYING == 1) return;

    IS_PLAYING = 1;

    pinMode(13, GPIO_OUTPUT);
    TickType_t startTime = xTaskGetTickCount();

    while (true)
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
        {
            IS_PLAYING = 0;
            break;
        }
    }
}
