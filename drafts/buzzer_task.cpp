//2023-07-30 22:27:19 - ESP32-C3 Tone BUzzer with duty cycle (based breathing led)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include <string.h> //memset
#include "esp_log.h"

static const char *TAG = "MyTask";



#define LED_PIN     18
#define LED_CHANNEL LEDC_CHANNEL_0
#define LED_FREQ_HZ 1200
#define LED_RESOLUTION LEDC_TIMER_10_BIT
#define FADE_DELAY_MS 10
#define ON_DELAY 300
#define OFF_DELAY 300


TaskHandle_t mainTaskHandle ;
// Fonction de la tâche
void task(void *pvParameters)
{
      puts("hello breathing word");

    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer));
    ledc_timer.duty_resolution = LED_RESOLUTION;
    ledc_timer.freq_hz = LED_FREQ_HZ;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;

    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel));

    ledc_channel.channel = LED_CHANNEL;
    ledc_channel.gpio_num = LED_PIN;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;

    // Configure LEDC Timer and Channel
    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);

    int duty=1023;
    duty = 100;
    int nTime = 2;
    for(int i=0; i<nTime; i++)
    {
           // Turn on buzzer
           puts("LED ON\n");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
            vTaskDelay(ON_DELAY / portTICK_PERIOD_MS);

            //2023-07-30 22:23:48 - Turn off buzzer, duty = 0
            puts("LED OFF\n");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
            vTaskDelay(OFF_DELAY / portTICK_PERIOD_MS);
    }


    // Supprimer la tâche
    vTaskDelete(NULL);
    //Faut rien mettre après ici, ce ne sera pas exécuté
}




extern "C" void app_main(void) {
        // Initialize logging
        esp_log_level_set(TAG, ESP_LOG_INFO); // Set log level to INFO
        mainTaskHandle = xTaskGetCurrentTaskHandle();
        // Créer la tâche
        puts("waiting");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        xTaskCreate(task, "myTask", 2048, NULL, 5, NULL);
        //xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);

        vTaskDelay(5000 / portTICK_PERIOD_MS);

        xTaskCreate(task, "myTask", 2048, NULL, 5, NULL);

          vTaskDelay(11000 / portTICK_PERIOD_MS);

        xTaskCreate(task, "myTask", 2048, NULL, 5, NULL);

        // Attendre la fin de la tâche
        puts("Wait for job task to finish\n");
       // xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        //vTaskDelete(NULL);
        puts("End main!\n");



        // // Faire respirer la LED de 0 à 100% d'intensité lumineuse
        // for (int duty = 0; duty <= maxDuty; duty++) {
        // ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, duty);
        // ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
        // vTaskDelay(FADE_DELAY_MS / portTICK_PERIOD_MS);
        // }


        // // Faire respirer la LED de 100% à 0% d'intensité lumineuse
        // for (int duty = maxDuty; duty >= 0; duty--) {
        // ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, duty);
        // ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
        // vTaskDelay(FADE_DELAY_MS / portTICK_PERIOD_MS);
        // }
}//app_name
