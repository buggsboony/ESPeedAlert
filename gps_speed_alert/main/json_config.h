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
    bool isValid;
    JTone():name("JTone untitled"),freq(555),enable(true),isValid(true){}        
}JTone;

//Point
typedef struct JPoint
{
    double X,Y;
    bool isValid;
    JPoint():X(0),Y(0),isValid(true){}   
}JPoint;    

//2023-09-24 18:03:32 - Polygon is made of multiple points
typedef struct JPolygon
{
    vector<JPoint>points;
    bool isValid;
    JPolygon():isValid(false){}
}JPolygon;

typedef struct JSector
{
    string name; //Sector name
    JPolygon polygon;
}JSector;


typedef struct JArea
{
    string name; //Area name
    string sectorName; //Sector name if any
    JSector sector;
    JPolygon polygon;
    JTone enter_tone,exit_tone; //Overload tones behaviours.
}JArea;



typedef struct JConfig
{
    JTone enter_tone,exit_tone;
    vector<JSector> sectors;
    vector<JArea> areas;
    double alt_prec;
    JConfig():alt_prec(4){}             
}JConfig;



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



// void JSON_Analyze(const cJSON * const root) {
// 	//ESP_LOGI(TAG, "root->type=%s", JSON_Types(root->type));
// 	cJSON *current_element = NULL;
// 	//ESP_LOGI(TAG, "roo->child=%p", root->child);
// 	//ESP_LOGI(TAG, "roo->next =%p", root->next);
// 	cJSON_ArrayForEach(current_element, root) {
// 		//ESP_LOGI(TAG, "type=%s", JSON_Types(current_element->type));
// 		//ESP_LOGI(TAG, "current_element->string=%p", current_element->string);
// 		if (current_element->string) {
// 			const char* string = current_element->string;
// 			ESP_LOGI(TAG, "[%s]", string);
// 		}
// 		if (cJSON_IsInvalid(current_element)) {
// 			ESP_LOGI(TAG, "Invalid");
// 		} else if (cJSON_IsFalse(current_element)) {
// 			ESP_LOGI(TAG, "False");
// 		} else if (cJSON_IsTrue(current_element)) {
// 			ESP_LOGI(TAG, "True");
// 		} else if (cJSON_IsNull(current_element)) {
// 			ESP_LOGI(TAG, "Null");
// 		} else if (cJSON_IsNumber(current_element)) {
// 			int valueint = current_element->valueint;
// 			double valuedouble = current_element->valuedouble;
// 			ESP_LOGI(TAG, "int=%d double=%f", valueint, valuedouble);
// 		} else if (cJSON_IsString(current_element)) {
// 			const char* valuestring = current_element->valuestring;
// 			ESP_LOGI(TAG, "%s", valuestring);
// 		} else if (cJSON_IsArray(current_element)) {
// 			//ESP_LOGI(TAG, "Array");
// 			JSON_Analyze(current_element);
// 		} else if (cJSON_IsObject(current_element)) {
// 			//ESP_LOGI(TAG, "Object");
// 			JSON_Analyze(current_element);
// 		} else if (cJSON_IsRaw(current_element)) {
// 			ESP_LOGI(TAG, "Raw(Not support)");
// 		}
// 	}
// }


//2023-09-24 17:18:54 - PArse some json element
string jsonReadString( cJSON * obj , string key, bool & success)
{
    string typedRes= "";
    success = false;
    cJSON * res = cJSON_GetObjectItem(obj,key.c_str());    
    if(res)
    {
        typedRes = res->valuestring;
        success = true;
    }
    return typedRes;
}//jsonReadString




//2023-09-24 17:18:54 - PArse some json element
double jsonReadDouble( cJSON * obj , string key, bool & success)
{
    double typedRes= -1;
    success = false;
    cJSON * res = cJSON_GetObjectItem(obj,key.c_str());    
    if(res)
    {
        if (cJSON_IsNumber(res)) {
          typedRes = res->valuedouble;
          success = true;      
        }        
    }
    return typedRes;
}//JsonReadDouble

//2023-10-14 09:42:00 - Just read value
double jsonReadDoubleValue( cJSON * obj , bool & success)
{
    double typedRes= -1;
    success = false;
//    cJSON * res = cJSON_GetObjectItem(obj,key.c_str()); 
    cJSON *res = obj;   
    if(res)
    {
        if (cJSON_IsNumber(res)) {
          typedRes = res->valuedouble;
          success = true;      
        }        
    }
    return typedRes;
}//JsonReadDouble



//2023-09-24 17:18:54 - PArse some json element
bool jsonReadBoolean(cJSON * obj ,string key, bool & success)
{  
    success = false;
    cJSON * res = cJSON_GetObjectItem(obj, key.c_str() );  
    if(res)
    {
        success = true;
        if ( cJSON_IsTrue(res) == 1) 
        {
            return true;
        } else {
            return false;
        }
    }
    return false;
}//jsonReadBoolean




//2023-09-24 17:18:54 - PArse some json element jsonReadPoint
JPoint jsonReadPoint( cJSON * obj , string key, bool & success)
{
 
    //                         [
    //                             4.6323036,
    //                             -1.00874
    //                         ] 
    success = false;
    JPoint jpoint;
    cJSON * res = cJSON_GetObjectItem(obj,key.c_str());    
    if(res)
    {
        double X=-1, Y=-1;
        short c=0;
        cJSON_ArrayForEach(res, obj) 
        {
            success=true;
            double d = jsonReadDoubleValue(res,success);
            if (success) {                
                if(c==0)
                {
                    X = d;
                }
                if(c==1)
                {
                    Y = d;
                }
                //ESP_LOGI(TAG, "int=%d double=%f", valueint, valuedouble);                
            }else
            {
                //ERror  !
            }
          c++;
        }//loop array   
        jpoint.X = X; jpoint.Y=Y;            
    }
    return jpoint;
}//jsonReadPoint

//2023-09-24 17:18:54 - PArse some json element
JPolygon jsonReadPolygon( cJSON * obj , string key, bool & success)
{
    // [
    //                         [
    //                             4.6323036,
    //                             -1.00874
    //                         ],
    //                         [
    //                             4.63306036,
    //                             -1.008
    //                         ],
    //                         [
    //                             4.63339823,
    //                             -1
    //                         ],
    //                         [
    //                             4.6351853,
    //                             -1.0091
    //                         ]
    //                     ]    
    success = false;
    JPolygon polygon;
    cJSON * res = cJSON_GetObjectItem(obj,key.c_str());    
    if(res)
    {
        cJSON_ArrayForEach(res, obj) 
        {
            success=true;
            JPoint point = jsonReadPoint(res,key,success);
            //if(success)
            {
                polygon.points.push_back(point);
            }
        }//loop array   
    }
    return polygon;
}//jsonReadPolygon


//2023-09-24 17:18:54 - PArse some json element
JSector jsonReadSector(cJSON * obj, bool & success)
{  
    // {
    //                     "name": "north",
    //                     "polygon": [
    //                         [
    //                             4.6323036,
    //                             -1.00874
    //                         ],
    //                         [
    //                             4.63306036,
    //                             -1.008
    //                         ],
    //                         [
    //                             4.63339823,
    //                             -1
    //                         ],
    //                         [
    //                             4.6351853,
    //                             -1.0091
    //                         ]
    //                     ]
    //                 }
    JSector sector;
    success = false;
 
    if (cJSON_IsObject(obj)) {
        //ESP_LOGI(TAG, "Object");
        bool succeeded = false;
        sector.name = jsonReadString(obj, "name",succeeded);        
             ESP_LOGI(TAG, "sector name is %s", sector.name.c_str());

        sector.polygon = jsonReadPolygon(obj,"polygon",succeeded);
        success = succeeded;        
    } 
    
    return sector;    
}//jsonReadSector


//2023-09-24 13:47:07 - Convert JSon object to object
void * convertJsonObject(cJSON * obj , string code_name)
{
    bool succeeded = false;
    double d;
    bool b;
    cJSON *current_element = NULL;

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

        d = jsonReadDouble(obj, "freq",succeeded);        
        if(succeeded) { tone->freq = d; } else tone->isValid = false; 

        b = jsonReadBoolean(obj, "enable",succeeded);        
        if(succeeded) { tone->enable = b; } else tone->isValid = false; 
    
        out = (void*) tone;
    }//endif "TONE"
  
    if(code_name=="alt_prec")
    {
        //Single Double   
         d = jsonReadDouble(obj, "freq",succeeded);        
        if(succeeded) 
        { 
             out = (void *) &d; //Return alt_prec            
        }        
    }
    
    if(code_name=="sectors")
    {
        ESP_LOGI(TAG, "--SECTORS--");
        //containing an array of sectors
        vector<JSector> *sectors = new vector<JSector>();
        cJSON_ArrayForEach(current_element, obj) 
        {
            JSector sector = jsonReadSector(current_element,succeeded);
            sectors->push_back(sector);
        }//loop array
        out = (void *) sectors;
    }
    
    if(code_name=="polygon")
    {
        //Tableau de double
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

