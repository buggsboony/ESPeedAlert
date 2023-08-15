#include <stdio.h>
#include <stdlib.h>
#define CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY 1
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"

//------------------------------ Buzzer ---------------
#include "buzzer_task.h"
//------------------------------ Buzzer ------------------


//2023-08-15 10:52:30 - Log Tag
static const char *TAG = "MyTask";


extern "C" void app_main(void)
{
       // // Initialize logging
        esp_log_level_set(TAG, ESP_LOG_INFO); // Set log level to INFO
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


    puts("END main !");
}//app_main
