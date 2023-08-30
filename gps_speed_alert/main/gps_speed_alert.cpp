#include <stdio.h>
#include <stdlib.h>
#define CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY 1
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"


#include "esp_h/esp_iotools.h" //2023-08-16 17:25:12 - to use functions : replaceStr()

//2023-08-15 10:52:30 - Log Tag
static const char *TAG = "ESPeedApp";


//------------------------------ nvs in esp_tools ---------------
#include "esp_tools.h" // nvs + led strip 
//------------------------------ nvs in esp_tools ------------------

//------------------------------ Buzzer ---------------
#include "buzzer_task.h"
//------------------------------ Buzzer ------------------

//------------------------------ Wifi & HTTP ---------------
#include "web_http_post.h"
//------------------------------ Wifi & HTTP ------------------

//----------------- Task Stuff for double_reset
TaskHandle_t mainTaskHandle;
nvs_handle_t nvsHandle;
TickType_t tickCount;
//----------------------------------- /LED Stuff
static led_strip_handle_t led_strip;
static uint8_t s_led_state = 0;

//---------------------- Web server Vars 
static httpd_handle_t server = NULL;


// Fonction de la tâche
void task(void *pvParameters)
{
    // Tâche en cours d'exécution
    printf("Tâche en cours...\n");

    // Simuler un travail
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Fin de la tâche
    printf("Tâche terminée.\n");

    // Supprimer la tâche
    vTaskDelete(NULL);
   //Faut rien mettre après ici, ce ne sera pas exécuté
}


//2023-07-12 19:37:40
TaskHandle_t jobHandle;
void mainJob(void *pvParameters)//*******************************************  main Job Task
{
    // Tâche en cours d'exécution
    ESP_LOGI(TAG,"mainJob() STARTED\n");
    

    //2023-08-17 19:28:51 - Turn on led with orange color
    s_led_state=1;
    turn_RGB_led(led_strip,s_led_state ,80,30,10); //RGB orange for running JOB
    ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS ON", s_led_state);
    

    //2023-08-17 18:51:09 - Turn on buzzer with config tone Morse code .--- for J (Job)  just ----
    tone_pattern_t tone_pattern_flash ;
    tone_pattern_flash.on_MS = 100;
    tone_pattern_flash.off_MS = 100;
    tone_pattern_flash.repeat = 4;
    tone_buzzer(tone_pattern_flash);


    vTaskDelay(1*1000/portTICK_PERIOD_MS);
    s_led_state=0; //Turn off the light
    turn_RGB_led(led_strip,s_led_state);
    ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS OFF", s_led_state);
    vTaskDelay(5*1000/portTICK_PERIOD_MS);


    for(int i=0; i<20; i++)
    {
        ESP_LOGI(TAG,">%d",i);
        // Simuler un travail
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // Fin de la tâche
    ESP_LOGI(TAG,"Job ended.\n");

    // Send notification to waiting task
    xTaskNotifyGive(mainTaskHandle);

    // Supprimer la tâche
    //vTaskDelete(NULL);  
}//mainJob



//Your configuration program (Simulation) ***************************************** CONFIG TASK ****************
TaskHandle_t configTaskHandle;
void configTask(void *pvParameters)
{    
    ESP_LOGI(TAG, "configTask(), ENTER CONFIG MODE HERE !");
    ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS ON", s_led_state);
    //vTaskDelay(pdMS_TO_TICKS(5510) );
    // vconfigTaskTaskDelay(pdMS_TO_TICKS(55100) ); // cout<<"wait 1 minute"<<endl;
     


    //2023-08-17 18:51:09 - Turn on buzzer with config tone Morse code _._. x2
    tone_pattern_t tone_pattern_flash ;
    tone_pattern_flash.on_MS = 200;
    tone_pattern_flash.off_MS = 50;
    tone_pattern_flash.repeat = 4;
    tone_buzzer(tone_pattern_flash);


   // //2023-08-17 18:50:53 - Turn on the light
    s_led_state=1;
    turn_RGB_led(led_strip,s_led_state ,2,2,60); //RGB blue for config color
    ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS ON", s_led_state);
    //vTaskDelay(4*1000/portTICK_PERIOD_MS);

     ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
        wifi_init_softap();
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));



    //// Send notification to waiting task    
    //xTaskNotifyGive(mainTaskHandle);
    //delete task
    //vTaskDelete(NULL);  
}//config task

//2023-07-14 09:21:29 - Wait a bit then lock state in NVS ********************
const int WAIT_LOCK_MS = 10100; //10 secs
void waitAndLockTask(void *pvParameters)
{
    ESP_LOGI(TAG,"Inside waitAndLockTask() %d sec \n",WAIT_LOCK_MS);

    vTaskDelay( WAIT_LOCK_MS / portTICK_PERIOD_MS);
    ESP_LOGI(TAG,"Exit waitAndLockTask() %d \n",WAIT_LOCK_MS);
    
    //2023-07-14 09:17:57 - now lock !
    nvs_write_string(nvsHandle,"reset_state","JOB");
    
    //delete task
    vTaskDelete(NULL);  
}




//2023-08-17 19:06:34 - init function
void initStuff()
{
    // Initialize logging
    esp_log_level_set(TAG, ESP_LOG_INFO);  // Set log level to INFO
    mainTaskHandle = xTaskGetCurrentTaskHandle();

    //2023-08-17 19:06:30 - Init led
    printf("Configuring LED strip portTICK_PERIOD_MS=%d\n", (int)portTICK_PERIOD_MS);
    configure_led(&led_strip);
 
    s_led_state=0; //Turn off the light
    turn_RGB_led(led_strip,s_led_state);
    ESP_LOG_LEVEL(ESP_LOG_WARN, TAG, "%d - LED IS OFF", s_led_state);
    vTaskDelay(5*1000/portTICK_PERIOD_MS);

    

    //Initialize NVS **************
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //2023-08-17 19:46:07 - open storage for config
    nvs_open("storage", NVS_READWRITE, &nvsHandle);
    

}//init






//Main app, entry point.
extern "C" void app_main(void)
{
    initStuff();



      configTask(NULL); //Force config task for tests
      return;















    // //ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));


    //RunTask to Wait then Write Lock
    xTaskCreate(waitAndLockTask, "waitAndLockTask", 2048, NULL, 5, NULL);

    //2023-07-14 09:12:15 - read reset_state
    string resetStateAtStart = nvs_read_string(nvsHandle,"reset_state","JOB");

    cout<<xTaskGetTickCount()<<" => resetStateAtStart : "<<resetStateAtStart<<endl;

    //2023-07-14 09:17:57 - write unlocked for now (config mode for a while)
    nvs_write_string(nvsHandle,"reset_state","CONFIG");

    if( resetStateAtStart == "CONFIG")
    {        
        //2023-07-14 09:25:09 - enter config mode (task)
        
        //async task gives overflow error when use Wifi
        //xTaskCreate(configTask, "configTask", 2048, NULL, 5, &configTaskHandle);
        nvs_write_string(nvsHandle,"reset_state","JOB"); //Set to locked state (run job state)        
        configTask(NULL); //Run it the sync way                
    }else// resetStateAtStart is "LOCKED" (JOB)
    {
        //Start main job (Blocking)
        mainJob(NULL);
        // xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);
        // cout<<"Wait main job to end !!!"<<endl;
        // xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        // cout<<"Job ended, let's finish..."<<endl;
    }

    cout<<" END PROGRAM "<<endl;
}




 