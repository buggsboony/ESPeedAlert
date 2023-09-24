
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



















static const char *TAG = "JSON";

char *JSON_Types(int type) {
	if (type == cJSON_Invalid) return ("cJSON_Invalid");
	if (type == cJSON_False) return ("cJSON_False");
	if (type == cJSON_True) return ("cJSON_True");
	if (type == cJSON_NULL) return ("cJSON_NULL");
	if (type == cJSON_Number) return ("cJSON_Number");
	if (type == cJSON_String) return ("cJSON_String");
	if (type == cJSON_Array) return ("cJSON_Array");
	if (type == cJSON_Object) return ("cJSON_Object");
	if (type == cJSON_Raw) return ("cJSON_Raw");
	return NULL;
}


void JSON_Analyze(const cJSON * const root) {
	//ESP_LOGI(TAG, "root->type=%s", JSON_Types(root->type));
	cJSON *current_element = NULL;
	//ESP_LOGI(TAG, "roo->child=%p", root->child);
	//ESP_LOGI(TAG, "roo->next =%p", root->next);
	cJSON_ArrayForEach(current_element, root) {
		//ESP_LOGI(TAG, "type=%s", JSON_Types(current_element->type));
		//ESP_LOGI(TAG, "current_element->string=%p", current_element->string);
		if (current_element->string) {
			const char* string = current_element->string;
			ESP_LOGI(TAG, "[%s]", string);
		}
		if (cJSON_IsInvalid(current_element)) {
			ESP_LOGI(TAG, "Invalid");
		} else if (cJSON_IsFalse(current_element)) {
			ESP_LOGI(TAG, "False");
		} else if (cJSON_IsTrue(current_element)) {
			ESP_LOGI(TAG, "True");
		} else if (cJSON_IsNull(current_element)) {
			ESP_LOGI(TAG, "Null");
		} else if (cJSON_IsNumber(current_element)) {
			int valueint = current_element->valueint;
			double valuedouble = current_element->valuedouble;
			ESP_LOGI(TAG, "int=%d double=%f", valueint, valuedouble);
		} else if (cJSON_IsString(current_element)) {
			const char* valuestring = current_element->valuestring;
			ESP_LOGI(TAG, "%s", valuestring);
		} else if (cJSON_IsArray(current_element)) {
			//ESP_LOGI(TAG, "Array");
			JSON_Analyze(current_element);
		} else if (cJSON_IsObject(current_element)) {
			//ESP_LOGI(TAG, "Object");
			JSON_Analyze(current_element);
		} else if (cJSON_IsRaw(current_element)) {
			ESP_LOGI(TAG, "Raw(Not support)");
		}
	}
}


void app_main_json()
{
	ESP_LOGI(TAG, "Serialize.....");
	cJSON *root;
	root = cJSON_CreateObject();
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	cJSON_AddStringToObject(root, "version", IDF_VER);
	cJSON_AddNumberToObject(root, "cores", chip_info.cores);
	cJSON_AddTrueToObject(root, "true");
	cJSON_AddFalseToObject(root, "false");
	cJSON_AddBoolToObject(root, "bool", 0);
	cJSON_AddNullToObject(root, "null");
	//const char *my_json_string = cJSON_Print(root);
	char *my_json_string = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",my_json_string);
	cJSON_Delete(root);

	ESP_LOGI(TAG, "Deserialize.....");
	cJSON *root2 = cJSON_Parse(my_json_string);
	JSON_Analyze(root2);
	cJSON_Delete(root2);

	// Buffers returned by cJSON_Print must be freed by the caller.
	// Please use the proper API (cJSON_free) rather than directly calling stdlib free.
	cJSON_free(my_json_string);
}
