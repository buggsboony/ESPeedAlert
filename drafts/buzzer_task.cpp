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




//2023-08-01 01:27:04 - Pattern buzzer
typedef struct tone_pattern_t
{
    int duty;
    int repeat;
    int on_MS;
    int off_MS;
    int freq;
    tone_pattern_t():duty(100),repeat(3),on_MS(300),off_MS(300),freq(1000){}
}tone_pattern_t;


// buzzer tone task
void buzzer_tone(void *pvParameters)
{
    puts("hello breathing word");
    tone_pattern_t tone_pattern; //default constructor values
    if( pvParameters != NULL)
    {
        tone_pattern = *(   (tone_pattern_t*) pvParameters );
    }


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

    for(int i=0; i<tone_pattern.repeat; i++)
    {
           // Turn on buzzer
           puts("BUZ ON\n");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, tone_pattern.duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
            vTaskDelay(tone_pattern.on_MS / portTICK_PERIOD_MS);

            //2023-07-30 22:23:48 - Turn off buzzer, duty = 0
            puts("BUZ OFF\n");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
            vTaskDelay(tone_pattern.off_MS / portTICK_PERIOD_MS);
    }


    // Supprimer la tâche
    vTaskDelete(NULL);
    //Faut rien mettre après ici, ce ne sera pas exécuté
}//buzzer_tone task


//2023-08-01 01:43:57 - Quickly start tone task
void tone_buzzer(tone_pattern_t tone_pattern)
{
        xTaskCreate(buzzer_tone, "buzzer_tone", 2048, (void *)&tone_pattern, 5, NULL);
}//tone_buzzer

extern "C" void app_main(void) {
        // Initialize logging
        esp_log_level_set(TAG, ESP_LOG_INFO); // Set log level to INFO
        mainTaskHandle = xTaskGetCurrentTaskHandle();
        // Créer la tâche
        puts("waiting");
        vTaskDelay(2000 / portTICK_PERIOD_MS);


        tone_pattern_t tone_pattern;
        tone_pattern.on_MS = 1000;
        tone_pattern.off_MS = 300;
        tone_buzzer(tone_pattern);

        vTaskDelay(4000 / portTICK_PERIOD_MS);

        tone_pattern_t tone_pattern_flash ;
        tone_pattern_flash.on_MS = 200;
        tone_pattern_flash.off_MS = 200;
        tone_pattern_flash.repeat = 6;
        tone_buzzer(tone_pattern_flash);

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
