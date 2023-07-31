#include <stdio.h>
#include <stdlib.h>
#define CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY 1
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"

//------------------------------ Buzzer ---------------
#define SAMPLE_CNT 32
static const adc_channel_t adc_channel ;
//static const adc1_channel_t adc1_channel;
static const adc2_channel_t adc2_channel = ADC2_CHANNEL_0;
//#define LEDC_GPIO 27 //ESP32 Standard Buzzer is connected to this PIN
//static const adc_channel_t adc_channel = ADC_CHANNEL_4;
#define LEDC_GPIO 19 //ESP32-C3 Standard Buzzer is connected to this PIN
//Please use dependency:
//idf.py add-dependency led_strip 
static ledc_channel_config_t ledc_channel;
#include "buzzer.h"
//--------------------------------------------------



extern "C" void app_main(void)
{
   printf("portTICK_RATE_MS= %ld \n",portTICK_RATE_MS); //portTICK_RATE_MS= 10
   printf("portTICK_PERIOD_MS= %ld \n",portTICK_PERIOD_MS); //portTICK_PERIOD_MS= 10

   //2023-07-30 11:49:17 - prepare buzzer
   init_hw_c3();
    esp_err_t ret;
    int adc1_reading[3] = {0xcc};
    int adc2_reading[1] = {0xcc};
   while (true)
   {
       uint32_t adc_val = 0;
       for (int i = 0; i < SAMPLE_CNT; ++i)
       {
           //adc_val += adc2_get_raw(adc_channel);
           adc_val += adc2_get_raw(adc2_channel , ADC_WIDTH_BIT_12, &adc2_reading[0]);
       }
       adc_val /= SAMPLE_CNT;

       ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, adc_val);
       ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
       //vTaskDelay(100 / portTICK_RATE_MS);
       vTaskDelay(300 / portTICK_PERIOD_MS);
   }//wend

}//app_main
