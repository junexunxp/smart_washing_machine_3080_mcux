#include "infra_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "arm_math.h"
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "infra_compat.h"
#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
#endif
#include "dev_model_api.h"
#include "dm_wrapper.h"
#include "cJSON.h"
#ifdef ATM_ENABLED
    #include "at_api.h"
#endif

#include "board.h"
#include "fsl_gpio.h"

#define EXAMPLE_TRACE(...)                                          \
    do {                                                            \
        HAL_Printf("\033[1;32;40m%s.%d: ", __func__, __LINE__);     \
        HAL_Printf(__VA_ARGS__);                                    \
        HAL_Printf("\033[0m\r\n");                                  \
    } while (0)

#define EXAMPLE_MASTER_DEVID            (0)
#define EXAMPLE_YIELD_TIMEOUT_MS        (100)

typedef struct {
    int master_devid;
    int cloud_connected;
    int master_initialized;
    int LightSwitch;
} wm_example_ctx_t;

static wm_example_ctx_t g_wm_ctx;
#if 0
#define PROPERTIY_WSWITCH			"WorkSwitch"
#define PROPERTIY_WSTATE			"WorkState"
#define PROPERTIY_CLSWITCH			"ChildLockSwitch"
#define PROPERTIY_WLEVEL			"WaterLevel"
#define PROPERTIY_LTIME 			"LeftTime"
#define PROPERTIY_SOTIME 			"SoakTime"
#define PROPERTIY_WTIME 			"WashTime"
#define PROPERTIY_RTIME 			"RinshTime"
#define PROPERTIY_SPTIME 			"SpinTime"
#define PROPERTIY_RTIMES 			"RinshTimes"
#define PROPERTIY_TSSPEED 			"TargetSpinSpeed"
#define PROPERTIY_TWTEM 			"TargetWaterTemperature"
#define PROPERTIY_DTIME 			"DryTime"
#define PROPERTIY_DSWITCH 			"DrySwitch"
#define PROPERTIY_TDETERGENT 		"TargetDetergent"
#define PROPERTIY_TSOFTENER 		"TargetSoftener"
#define PROPERTIY_TDISINFECTAN 		"TargetDisinfectant"
#define PROPERTIY_DOSTATE  		    "DoorOpeningState"
#define PROPERTIY_PASWITCH  		    "PauseSwitch"
#define PROPERTIY_DOPT  		    "DryOpt"
#define PROPERTIY_RTIMER  		    "ReservationTimer"
#define PROPERTIY_POSWITCH  		    "PowerSwitch"
#else
typedef enum{
	PROPERTIY_WSWITCH,
	PROPERTIY_WSTATE,
	PROPERTIY_CLSWITCH,
	PROPERTIY_WLEVEL,
	PROPERTIY_LTIME,
	PROPERTIY_SOTIME,
	PROPERTIY_WTIME,
	PROPERTIY_RTIME,
	PROPERTIY_SPTIME,
	PROPERTIY_RTIMES,
	PROPERTIY_TSSPEED,
	PROPERTIY_TWTEM,
	PROPERTIY_DTIME,
	PROPERTIY_DSWITCH,
	PROPERTIY_TDETERGENT,
	PROPERTIY_TSOFTENER,
	PROPERTIY_TDISINFECTAN,
	PROPERTIY_DOSTATE,
	PROPERTIY_PASWITCH,
	PROPERTIY_DOPT,
	PROPERTIY_RTIMER,
	PROPERTIY_POSWITCH,
	PROPERITY_ALL,
}wm_propertity_e;
const char *wm_properties[] = {"WorkSwitch","WorkState","ChildLockSwitch","WaterLevel","LeftTime",
	"SoakTime","WashTime","RinshTime","SpinTime","RinshTimes","TargetSpinSpeed","TargetWaterTemperature",
	"DryTime","DrySwitch","TargetDetergent","TargetSoftener","TargetDisinfectant","DoorOpeningState",
	"PauseSwitch","DryOpt","ReservationTimer","PowerSwitch"};



#endif
typedef enum{
	WS_IDLE = 0,
	WS_WORKING,
	WS_FINISHED,
	WS_RESERVATION,
	WS_PAUSE,
	WS_ERROR,
	WS_SHUTDOWN
}wm_work_state_e;

typedef enum{
	WL_LOW = 1,
	WL_MIDDLE,
	WL_HIGH

}wm_water_level_e;
typedef enum{
	DO_NONE,
	DO_WEAK,
	DO_MIDDLE,
	DO_HIGH
}wm_dry_opt_e;


typedef struct{
	bool work_switch;
	wm_work_state_e work_state;
	bool clock_switch;
	wm_water_level_e water_level;
	float left_time;
	float soak_time;
	float wash_time;
	float rinsh_time;
	float spin_time;
	int32_t rinsh_times;
	int32_t target_ss;
	float target_wtem;
	float dry_time;
	bool dry_switch;
	float target_detergent;
	float target_softener;
	float target_disinfectant;
	bool door_opening_state;
	bool pause_switch;
	wm_dry_opt_e dry_opt;
	float reserv_time;
	bool power_switch;
}wm_data_info_t;

static wm_data_info_t wm_ib={
	.water_level = WL_HIGH,

	.target_wtem = 55,

};



/** cloud connected event callback */
static int wm_connected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Connected");
    g_wm_ctx.cloud_connected = 1;

    return 0;
}

/** cloud disconnected event callback */
static int wm_disconnected_event_handler(void)
{
    EXAMPLE_TRACE("Cloud Disconnected");
    g_wm_ctx.cloud_connected = 0;

    return 0;
}

/* device initialized event callback */
static int wm_initialized(const int devid)
{
    EXAMPLE_TRACE("Device Initialized");
    g_wm_ctx.master_initialized = 1;

    return 0;
}

/** recv property post response message from cloud **/
static int wm_report_reply_event_handler(const int devid, const int msgid, const int code, const char *reply,
        const int reply_len)
{
    char *p_buffer;
    if (reply != NULL) {
        p_buffer = HAL_Malloc(reply_len + 1);
        memcpy(p_buffer, reply, reply_len);
        p_buffer[reply_len] = '\0';
    }
    
    EXAMPLE_TRACE("Message Post Reply Received, Message ID: %d, Code: %d, Reply: %s", msgid, code,
                  (reply == NULL)? ("NULL") : (p_buffer));
    
    if (reply != NULL) {
        HAL_Free(p_buffer);
    }
    return 0;
}

/** recv event post response message from cloud **/
static int wm_trigger_event_reply_event_handler(const int devid, const int msgid, const int code, const char *eventid,
        const int eventid_len, const char *message, const int message_len)
{
    EXAMPLE_TRACE("Trigger Event Reply Received, Message ID: %d, Code: %d, EventID: %.*s, Message: %.*s",
                  msgid, code,
                  eventid_len,
                  eventid, message_len, message);

    return 0;
}

		
static void wm_property_ib_set(wm_propertity_e epro, cJSON *cvalue){
	switch(epro){
		case PROPERTIY_WSWITCH:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.work_switch = cvalue->valueint;
			
		}
		break;
		case PROPERTIY_WSTATE:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.work_state = cvalue->valueint;


		}
		break;
		case PROPERTIY_CLSWITCH:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.clock_switch = cvalue->valueint;


		}
		break;
		case PROPERTIY_WLEVEL:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.water_level = cvalue->valueint;

		}
		break;
		case PROPERTIY_LTIME:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.left_time = cvalue->valuedouble;
		}
		break;
		case PROPERTIY_SOTIME:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.soak_time = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_WTIME:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.wash_time = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_RTIME:{
			
		if(cJSON_IsNumber(cvalue))
			wm_ib.rinsh_time = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_SPTIME:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.spin_time = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_RTIMES:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.rinsh_times = cvalue->valueint;

		}
		break;
		case PROPERTIY_TSSPEED:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.target_ss = cvalue->valuedouble;
	

		}
		break;
		case PROPERTIY_TWTEM:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.target_wtem = cvalue->valuedouble;
		}
		break;
		case PROPERTIY_DTIME:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.dry_time = cvalue->valuedouble;
		}
		break;
		case PROPERTIY_DSWITCH:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.dry_switch = cvalue->valueint;
		}
		break;
		case PROPERTIY_TDETERGENT:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.target_detergent = cvalue->valuedouble;
		}
		break;
		case PROPERTIY_TSOFTENER:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.target_softener = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_TDISINFECTAN:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.target_disinfectant = cvalue->valuedouble;

		}
		break;
		case PROPERTIY_DOSTATE:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.door_opening_state = cvalue->valueint;

		}
		break;
		case PROPERTIY_PASWITCH:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.pause_switch = cvalue->valueint;
		}
		break;
		case PROPERTIY_DOPT:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.dry_opt = cvalue->valueint;

		}
		break;
		case PROPERTIY_RTIMER:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.reserv_time = cvalue->valuedouble;
	
		}
		break;
		case PROPERTIY_POSWITCH:{
			if(cJSON_IsNumber(cvalue))
				wm_ib.power_switch = cvalue->valueint;
		}
		break;
		default:{
		break;
		}
        }

}

static int wm_property_hal_set(cJSON *proot){
	cJSON *p_setv;
	int i;
	for(i=0;i<PROPERITY_ALL;i++){
		p_setv = cJSON_GetObjectItemCaseSensitive(proot, wm_properties[i]);
		if(p_setv){
			break;
		}
	}
	if(p_setv){

		wm_property_ib_set(i,p_setv);

	}



}

/** recv event post response message from cloud **/
static int wm_property_set_event_handler(const int devid, const char *request, const int request_len)
{
    int res = 0;
    EXAMPLE_TRACE("Property Set Received, Request: %s", request);
    
    cJSON *p_root;
    
    p_root = cJSON_Parse(request);
    if (p_root == NULL || !cJSON_IsObject(p_root)) {
        HAL_Printf("JSON Parse Error\r\n");
        return -1;
    }
    
    wm_property_hal_set(p_root);
    
    cJSON_Delete(p_root);

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
                             (unsigned char *)request, request_len);
    EXAMPLE_TRACE("Post Property Message ID: %d", res);

    return 0;
}


static int wm_service_request_event_handler(const int devid, const char *serviceid, const int serviceid_len,
                                              const char *request, const int request_len,
                                              char **response, int *response_len)
{
    int add_result = 0;
    cJSON *root = NULL, *item_number_a = NULL, *item_number_b = NULL;
    const char *response_fmt = "{\"Result\": %d}";

    EXAMPLE_TRACE("Service Request Received, Service ID: %.*s, Payload: %s", serviceid_len, serviceid, request);

    /* Parse Root */
    root = cJSON_Parse(request);
    if (root == NULL || !cJSON_IsObject(root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return -1;
    }

    if (strlen("Operation_Service") == serviceid_len && memcmp("Operation_Service", serviceid, serviceid_len) == 0) {
        /* Parse NumberA */
        item_number_a = cJSON_GetObjectItem(root, "NumberA");
        if (item_number_a == NULL || !cJSON_IsNumber(item_number_a)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberA = %d", item_number_a->valueint);

        /* Parse NumberB */
        item_number_b = cJSON_GetObjectItem(root, "NumberB");
        if (item_number_b == NULL || !cJSON_IsNumber(item_number_b)) {
            cJSON_Delete(root);
            return -1;
        }
        EXAMPLE_TRACE("NumberB = %d", item_number_b->valueint);

        add_result = item_number_a->valueint + item_number_b->valueint;

        /* Send Service Response To Cloud */
        *response_len = strlen(response_fmt) + 10 + 1;
        *response = (char *)HAL_Malloc(*response_len);
        if (*response == NULL) {
            EXAMPLE_TRACE("Memory Not Enough");
            return -1;
        }
        memset(*response, 0, *response_len);
        HAL_Snprintf(*response, *response_len, response_fmt, add_result);
        *response_len = strlen(*response);
    }

    cJSON_Delete(root);
    return 0;
}

static int wm_timestamp_reply_event_handler(const char *timestamp)
{
    EXAMPLE_TRACE("Current Timestamp: %s", timestamp);

    return 0;
}

/** fota event handler **/
static int wm_fota_event_handler(int type, const char *version)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* 0 - new firmware exist, query the new firmware */
    if (type == 0) {
        EXAMPLE_TRACE("New Firmware Version: %s", version);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_FOTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

/* cota event handler */
static int wm_cota_event_handler(int type, const char *config_id, int config_size, const char *get_type,
                                   const char *sign, const char *sign_method, const char *url)
{
    char buffer[128] = {0};
    int buffer_length = 128;

    /* type = 0, new config exist, query the new config */
    if (type == 0) {
        EXAMPLE_TRACE("New Config ID: %s", config_id);
        EXAMPLE_TRACE("New Config Size: %d", config_size);
        EXAMPLE_TRACE("New Config Type: %s", get_type);
        EXAMPLE_TRACE("New Config Sign: %s", sign);
        EXAMPLE_TRACE("New Config Sign Method: %s", sign_method);
        EXAMPLE_TRACE("New Config URL: %s", url);

        IOT_Linkkit_Query(EXAMPLE_MASTER_DEVID, ITM_MSG_QUERY_COTA_DATA, (unsigned char *)buffer, buffer_length);
    }

    return 0;
}

static int wm_build_property_name_value(char *out, wm_propertity_e epro){
	int offset = 0;
	if(epro >= PROPERITY_ALL){

		return -1;
	}
	offset += HAL_Snprintf(out + offset,64, "{\"%s\": ", wm_properties[epro]);
	switch(epro){
		case PROPERTIY_WSWITCH:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.work_switch);

		}
		break;
		case PROPERTIY_WSTATE:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.work_state);


		}
		break;
		case PROPERTIY_CLSWITCH:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.clock_switch);


		}
		break;
		case PROPERTIY_WLEVEL:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.water_level);


		}
		break;
		case PROPERTIY_LTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.left_time);


		}
		break;
		case PROPERTIY_SOTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.soak_time);


		}
		break;
		case PROPERTIY_WTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.wash_time);


		}
		break;
		case PROPERTIY_RTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.rinsh_time);


		}
		break;
		case PROPERTIY_SPTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.spin_time);


		}
		break;
		case PROPERTIY_RTIMES:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.rinsh_times);


		}
		break;
		case PROPERTIY_TSSPEED:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.target_ss);


		}
		break;
		case PROPERTIY_TWTEM:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.target_wtem);

		}
		break;
		case PROPERTIY_DTIME:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.dry_time);

		}
		break;
		case PROPERTIY_DSWITCH:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.dry_switch);

		}
		break;
		case PROPERTIY_TDETERGENT:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.target_detergent);

		}
		break;
		case PROPERTIY_TSOFTENER:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.target_softener);

		}
		break;
		case PROPERTIY_TDISINFECTAN:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.target_disinfectant);

		}
		break;
		case PROPERTIY_DOSTATE:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.door_opening_state);

		}
		break;
		case PROPERTIY_PASWITCH:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.pause_switch);

		}
		break;
		case PROPERTIY_DOPT:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.dry_opt);

		}
		break;
		case PROPERTIY_RTIMER:{
			offset += HAL_Snprintf(out + offset,64, "%f}", wm_ib.reserv_time);

		}
		break;
		case PROPERTIY_POSWITCH:{
			offset += HAL_Snprintf(out + offset,64, "%d}", wm_ib.power_switch);

		}
		break;
		default:{
		break;
		}

	}
	return offset;

}

void wm_property_post(wm_propertity_e epro){
	char property_payload[512] = {0};
	int offset = 0;

	if(epro == PROPERITY_ALL){
		
		int i;
		for(i=0;i<PROPERITY_ALL;i++){
			offset = wm_build_property_name_value(property_payload+offset,(wm_propertity_e )i);
		}

	}else{
		offset = wm_build_property_name_value(property_payload,epro);

	}
	
	IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_POST_PROPERTY,
									 (unsigned char *)property_payload, strlen(property_payload));

}

void wm_post_event(void)
{
    int res = 0;
    char *event_id = "HardwareError";
    char *event_payload = "{\"ErrorCode\": 0}";

    res = IOT_Linkkit_TriggerEvent(EXAMPLE_MASTER_DEVID, event_id, strlen(event_id),
                                   event_payload, strlen(event_payload));
    EXAMPLE_TRACE("Post Event Message ID: %d", res);
}

void wm_deviceinfo_update(void)
{
    int res = 0;
    char *device_info_update = "[{\"attrKey\":\"abc\",\"attrValue\":\"hello,world\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_UPDATE,
                             (unsigned char *)device_info_update, strlen(device_info_update));
    EXAMPLE_TRACE("Device Info Update Message ID: %d", res);
}

void wm_deviceinfo_delete(void)
{
    int res = 0;
    char *device_info_delete = "[{\"attrKey\":\"abc\"}]";

    res = IOT_Linkkit_Report(EXAMPLE_MASTER_DEVID, ITM_MSG_DEVICEINFO_DELETE,
                             (unsigned char *)device_info_delete, strlen(device_info_delete));
    EXAMPLE_TRACE("Device Info Delete Message ID: %d", res);
}



int wm_run(int argc, char **argv)
{
    int res = 0;
    int cnt = 0;
    iotx_linkkit_dev_meta_info_t wm_meta_info;
    int domain_type = 0, dynamic_register = 0, post_reply_need = 0;

#ifdef ATM_ENABLED
      if (IOT_ATM_Init() < 0) {
              EXAMPLE_TRACE("IOT ATM init failed!\n");
      
      }
  #endif
  	app_wait_wifi_connect();
    

    gpio_pin_config_t led_config = {
      .direction = kGPIO_DigitalOutput,
      .outputLogic = 1U,
    };

    GPIO_PinInit(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, &led_config);    
    
    memset(&g_wm_ctx, 0, sizeof(wm_example_ctx_t));
    memset(&wm_meta_info, 0, sizeof(iotx_linkkit_dev_meta_info_t));

	HAL_GetProductKey(wm_meta_info.product_key);
	HAL_GetProductSecret(wm_meta_info.product_secret);
	HAL_GetDeviceName(wm_meta_info.device_name);
	HAL_GetDeviceSecret(wm_meta_info.device_secret);
   

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    /* Register Callback */
    IOT_RegisterCallback(ITE_CONNECT_SUCC, wm_connected_event_handler);
    IOT_RegisterCallback(ITE_DISCONNECTED, wm_disconnected_event_handler);
    IOT_RegisterCallback(ITE_SERVICE_REQUEST, wm_service_request_event_handler);
    IOT_RegisterCallback(ITE_PROPERTY_SET, wm_property_set_event_handler);
    IOT_RegisterCallback(ITE_REPORT_REPLY, wm_report_reply_event_handler);
    IOT_RegisterCallback(ITE_TRIGGER_EVENT_REPLY, wm_trigger_event_reply_event_handler);
    IOT_RegisterCallback(ITE_TIMESTAMP_REPLY, wm_timestamp_reply_event_handler);
    IOT_RegisterCallback(ITE_INITIALIZE_COMPLETED, wm_initialized);
    IOT_RegisterCallback(ITE_FOTA, wm_fota_event_handler);
    IOT_RegisterCallback(ITE_COTA, wm_cota_event_handler);

    domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login Method */
    dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    /* post reply doesn't need */
    post_reply_need = 1;
    IOT_Ioctl(IOTX_IOCTL_RECV_EVENT_REPLY, (void *)&post_reply_need);

    /* Create Master Device Resources */
    g_wm_ctx.master_devid = IOT_Linkkit_Open(IOTX_LINKKIT_DEV_TYPE_MASTER, &wm_meta_info);
    if (g_wm_ctx.master_devid < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Open Failed\n");
        return -1;
    }

    /* Start Connect Aliyun Server */
    res = IOT_Linkkit_Connect(g_wm_ctx.master_devid);
    if (res < 0) {
        EXAMPLE_TRACE("IOT_Linkkit_Connect Failed\n");
        return -1;
    }
	//wm_property_post(PROPERITY_ALL);

    while (1) {
        
        IOT_Linkkit_Yield(EXAMPLE_YIELD_TIMEOUT_MS);
        
        cnt++;

        /* Post Event Example */
        if ((cnt % 300) == 0) {
            wm_property_post(PROPERTIY_WLEVEL);
        }
    }
}
/* Task priorities. */
#define wm_runTASKPRIORITY (3)
void smart_wm_init(void ){
	xTaskCreate(wm_run, "wm_run_task", 2048 + 10, NULL, wm_runTASKPRIORITY, NULL);
}
