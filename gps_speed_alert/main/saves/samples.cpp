
// 2023-08-16 11:41:18 - Buzzer test
extern "C" void app_main_BUZZER(void)
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
}//app_main_buzzer




//2023-08-17 19:08:53 - Use NVS and tick count , double reset (2 behaviours)
extern "C" void app_main_double_reset(void)
{    
    // Initialize logging
    esp_log_level_set(TAG, ESP_LOG_INFO);  // Set log level to INFO
    mainTaskHandle = xTaskGetCurrentTaskHandle();


    nvs_flash_init();
    nvs_open("storage", NVS_READWRITE, &nvsHandle);


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
        xTaskCreate(configTask, "configTask", 2048, NULL, 5, &configTaskHandle);
        nvs_write_string(nvsHandle,"reset_state","JOB"); //Set to locked state (run job state)
        //BlinkBlue //config mode
        cout<<"Wait till manual restart or config end!!!"<<endl;
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        cout<<"Done Waiting config, let's exit"<<endl;
    }else// resetStateAtStart is "LOCKED" (JOB)
    {
        //Start main job (Blocking)
        xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);
        cout<<"Wait main job to end !!!"<<endl;
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        cout<<"Job ended, let's finish..."<<endl;
    }




 puts("End main !!!\n");
}//end main

