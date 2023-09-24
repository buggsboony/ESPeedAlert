//2023-09-01 13:36:08 - Read / Parse Config functions


#include "esp_log.h"
//#include "esp_chip_info.h"
#include "cJSON.h"



// //2023-09-24 12:59:41 - Définiton de JSON Tone
typedef struct JTone
{    
    string name;
    int freq;
    bool enable;
    JTone():name("JTone untitled"),freq(555),enable(true){}        
}JTone;


//2023-08-31 17:55:24 - Readable json type
string JSON_Types(int type) {
    string st ="cJSON_Unknown";
	if (type == cJSON_Invalid) st="cJSON_Invalid";
	if (type == cJSON_False) st="cJSON_False";
	if (type == cJSON_True) st="cJSON_True";
	if (type == cJSON_NULL) st="cJSON_NULL";
	if (type == cJSON_Number) st="cJSON_Number";
	if (type == cJSON_String) st="cJSON_String";
	if (type == cJSON_Array) st="cJSON_Array";
	if (type == cJSON_Object) st="cJSON_Object";
	if (type == cJSON_Raw) st= "cJSON_Raw";

	return st;
}


double jsonReadDouble(bool & success)
{
    double typedRes;
    success = false;
    cJSON * res = cJSON_GetObjectItem(obj,"freq");
    
    if (res){
     typedRes = res->valuedouble;
    }
    return typedRes;
}//JsonReadDouble
//2023-09-24 13:47:07 - Convert JSon object to object
void * convertJsonObject(cJSON * obj , string code_name)
{
    void * out = NULL;
    if( !obj ) return out;
    //Parse tone object
    if(code_name=="TONE")
    {
        //It's a JTone
        // Exemple : 
        //  "enter_tone": {
        //             "freq": 700,
        //             "enable": true
        //         },
        
        JTone * tone = new JTone();

        double d = jsonReadDouble(succeed);
            // double lng = cJSON_GetObjectItem(root,"lng")->valuedouble;

        out = (void*) tone;
    }
  
    if(code_name=="alt_prec")
    {
        //Single Double   
    }
    
    if(code_name=="sectors")
    {
        //Tableau de sector contenant "name" et "polygon"
    }
    
    if(code_name=="polygon")
    {
        //Tableau de double
    }
    
    if(code_name=="alt_prec")
    {
        
    }
    return out;
}//parseObject



    //2023-08-31 17:43:18 read JSON Config function
    void readJsonConfig()
    {
            string json = nvs_read_string(nvsHandle,"JSON_CONFIG");
    
        string json_test2 = R"(
                        {
                "enter_tone": {
                    "freq": 700,
                    "enable": true
                },
                "exit_tone": {
                    "freq": 500,
                    "enable": true
                },
                "alt_prec": 4,
                "sectors": [
                    {
                        "name": "north",
                        "polygon": [
                            [
                                4.6323036,
                                -1.00874
                            ],
                            [
                                4.63306036,
                                -1.008
                            ],
                            [
                                4.63339823,
                                -1
                            ],
                            [
                                4.6351853,
                                -1.0091
                            ]
                        ]
                    },
                    {
                        "name": "south",
                        "polygon": [
                            [
                                4.323036,
                                -1.0874
                            ],
                            [
                                4.3306036,
                                -1.08
                            ],
                            [
                                4.3339823,
                                -1
                            ],
                            [
                                4.351853,
                                -1.091
                            ]
                        ]
                    }
                ],
                "areas": [
                    {
                        "sector": "north",
                        "name": "Road 1",
                        "polygon": [
                            [
                                4.6323036,
                                -1.00874,
                                32
                            ],
                            [
                                4.63306036,
                                -1.008
                            ],
                            [
                                4.63339823,
                                -1
                            ],
                            [
                                4.6351853,
                                -1.0091
                            ],
                            null
                        ],
                        "exit_tone": {
                            "enable": false
                        }
                    }
                ]
            }
                        
        )";

            //ESP_LOGI(TAG, "my_json_string\n%s",json.c_str() );	
            ESP_LOGI(TAG, "Deserialize.....");
            cJSON *root = cJSON_Parse(json.c_str());
            ESP_LOGI(TAG, "root->type=%s", JSON_Types(root->type).c_str() );

            //Récupérer le enter_tone et exit_tone          
            cJSON *json_tone = cJSON_GetObjectItem(root,"enter_tone"); 
            void * aJtone = convertJsonObject(json_tone,"TONE");
            JTone * enter_tone = (JTone *) aJtone;

            ESP_LOGI(TAG, "Freq jtone freq=%d",enter_tone->freq);


            // double lat = cJSON_GetObjectItem(root,"lat")->valuedouble;
            // double lng = cJSON_GetObjectItem(root,"lng")->valuedouble;
            // ESP_LOGI(TAG, "Name=%s",name);
            // ESP_LOGI(TAG, "lng=%f",lng);
            // ESP_LOGI(TAG, "lat=%f",lat);

            // int root_array_size = cJSON_GetArraySize(root); 
            // ESP_LOGI(TAG, "root_array_size=%d", root_array_size);

            // for (int i=0;i<root_array_size;i++) {
            //     cJSON *array = cJSON_GetArrayItem(root,i);
            //     JSON_Print(array);
            // }
            //cJSON_Delete(root);
    }//readJsonConfig

