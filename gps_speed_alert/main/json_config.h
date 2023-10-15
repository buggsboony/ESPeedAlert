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

    string toString()
    {
    	// string str = "JTone #name=[";
        // str.append(name+"]");
        // str.append(" freq=[");
        // str.append(doubleToStr(freq));
        
        std::ostringstream oss;
        oss<<"JTone name=["<<name<<"] "<<"freq=["<<freq<<"]"
        <<" enable="<<enable<<" isValid="<<this->isValid<<endl;
        
        return oss.str();
    }
    JTone():name("JTone untitled"),freq(555),enable(true),isValid(true){}        
}JTone;

//Point
typedef struct JPoint
{
    double X,Y;
    bool isValid;
    JPoint():X(0),Y(0),isValid(true){}   

    string toString()
    {    	
        std::ostringstream oss;
        oss<<"JPoint("<<X<<","<<Y<<")->isValid="<<this->isValid<<endl;        
        return oss.str();
    }//toString()
}JPoint;    

//2023-09-24 18:03:32 - Polygon is made of multiple points
typedef struct JPolygon
{
    vector<JPoint>points;
    bool isValid;
    JPolygon():isValid(false){}

    string toString()
    {    	
        std::ostringstream oss;
        short n=0;
        oss<<"Polygon: "<<endl;
        for(JPoint point: points)
        {
            oss<<++n<<":"<<point.toString();        
        }//next point in polygon
        return oss.str();
    }//toString()
}JPolygon;

typedef struct JSector
{
    string name; //Sector name
    JPolygon polygon;
    
    string toString()
    {    	
        std::ostringstream oss;        
        oss<<"Sector: name=["<<name<<"], s.poly="<<polygon.toString()<<endl;       
        return oss.str();
    }//toString()
}JSector;


typedef struct JArea
{
    string name; //Area name
    string sectorName; //Sector name if any
    JSector sector;
    JPolygon polygon;
    JTone enter_tone,exit_tone; //Overload tones behaviours.

    string toString()
    {    	
        std::ostringstream oss;    
        oss<<"Area: name=["<<name<<"] "<<"a.Sector:"<<sector.toString()<<" a.poly:"<<polygon.toString()
        <<"enter_tone:"<<enter_tone.toString()<<"exit_tone:"<<exit_tone.toString()<<endl;       
        return oss.str();
    }//toString()
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
}//analkyz


//2023-10-14 09:42:00 - Just read value
double jsonReadDoubleValue( cJSON * current_element , bool & success)
{
    double typedRes= -1;
    success = false;

    if (cJSON_IsNumber(current_element)) {
        //int typedRes = json_item->valueint;
        typedRes = current_element->valuedouble;        
        success = true;
    } else 
    {        
        cout<<"IS NOT a Number"<<endl;
        JSON_Analyze(current_element);
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
    // cJSON * res = cJSON_GetObjectItem(obj,key.c_str());    
    // if(res)
    {
        
        short c=0; 
        double X=-1, Y=-1;
        cJSON *current_element = NULL;
        cJSON_ArrayForEach(current_element, obj) 
        {
         
            success=true;
            double d = jsonReadDoubleValue(current_element,success);
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
    cout<<"Reading polygon !"<<endl;
    if(res)
    {
        cJSON_ArrayForEach(res, obj) 
        {
            cJSON * current_element;
            cJSON_ArrayForEach(current_element, res)
            {           
                if (cJSON_IsNumber(current_element)) 
                {
                    int valueint = current_element->valueint;
                    double valuedouble = current_element->valuedouble;
                    ESP_LOGI(TAG, "---------->int=%d double=%f", valueint, valuedouble);
                }else
                {
                    cout<<"Analyzin!!!!!!!"<<endl;
            
                    success=false;
                    //JSON_Analyze(current_element);
                       // JPoint point = jsonReadPoint(res,key,success);
                    // cout<<"next point in array "<<point.toString()<<endl;
                    // //if(success)
                    // {
                    //     polygon.points.push_back(point);
                    // }
                    }
            }//next paire
         
        }//loop array   
    }
    return polygon;
}//jsonReadPolygon


//2023-09-24 17:18:54 - PArse some json element
JSector jsonReadSector(cJSON * obj, bool & success)
{  
    cout<<"Reading Sector !"<<endl;
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
         d = jsonReadDouble(obj, code_name ,succeeded);        
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

    JConfig config;

    //2023-08-31 17:43:18 read JSON Config function
    void readJsonConfig()
    {
            string json = nvs_read_string(nvsHandle,"JSON_CONFIG");   
            //ESP_LOGI(TAG, "my_json_string\n%s",json.c_str() );	
            ESP_LOGI(TAG, "Deserialize.....");
            cJSON *root = cJSON_Parse(json.c_str());
            ESP_LOGI(TAG, "root->type=%s", JSON_Types(root->type).c_str() );

            //Récupérer le enter_tone et exit_tone          
            cJSON* json_item; bool succeeded=false; double d=-1;
            
            json_item = cJSON_GetObjectItem(root,"enter_tone"); 
            if(json_item != NULL)
            {
                void * aJtone = convertJsonObject(json_item,"TONE");
                if(aJtone != NULL)
                {
                    config.enter_tone = *( (JTone*) aJtone );
                   // ESP_LOGI(TAG, "debug enter_tone: %s",config.enter_tone.toString().c_str() );
                }
            }//endif json_item
         
            json_item = cJSON_GetObjectItem(root,"exit_tone"); 
            if(json_item != NULL)
            {
                void * aJtone = convertJsonObject(json_item,"TONE");
                if(aJtone != NULL)
                {
                  config.enter_tone = *( (JTone*) aJtone );
                 // ESP_LOGI(TAG, "debug exit_tone: %s",config.enter_tone.toString().c_str() );
                }
            }//endif json_item
            //----------------------

            json_item = cJSON_GetObjectItem(root,"alt_prec"); 
            if(json_item != NULL)
            {
                d = jsonReadDoubleValue(json_item,succeeded);
                if(succeeded)
                {
                    config.alt_prec = d;
                    ESP_LOGI(TAG, "debug alt_prec: %2.2f",config.alt_prec );
                }
            }
            //-----------------------------

            //sectors
            json_item = cJSON_GetObjectItem(root,"sectors");
            if(json_item != NULL)
            {
                void * sectors = convertJsonObject(json_item,"sectors");
                config.sectors = *( (vector<JSector>*) sectors );

                //cout<<"Sectors count = "<<config.sectors.size()<<endl;
                cout<<"Sectors[0] = "<<config.sectors[0].toString()<<endl;

            }//sectors
            

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

