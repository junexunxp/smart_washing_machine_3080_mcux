/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include <stdarg.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
/*******************************************************************************
 * Variables
 ******************************************************************************/
#define MAX_SSID_SIZE  32
#define MAX_PWD_SIZE   64

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE+1)
#endif
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

#define KV_USED 0
void app_wait_wifi_connect(void ){
#if KV_USED
	char wifi_ssid[40]={0};
	char wifi_key[40] = {0};
	int ssid_len = 40;
	int key_len = 40;
	if(HAL_Kv_Get("wifi_ssid", wifi_ssid, &ssid_len) == 0){
          if(ssid_len != 1 || wifi_ssid[0] != 0xff){


            if(HAL_Kv_Get("wifi_key", wifi_key, &key_len) == 0){
               if(key_len != 1){
                  at_wifi_join(wifi_ssid,wifi_key);
                  HAL_Printf("join wifi:%s....\r\n",wifi_ssid);
                  HAL_SleepMs(2000);
               }
            }
          }
	}
#endif
	if(!HAL_Wifi_Connected()){
	    HAL_Printf("Wifi not connected, join the AP first\r\n");
	    HAL_SleepMs(1000);
	    while(!HAL_Wifi_Connected()){
	      HAL_SleepMs(500);
	    }
	}

}
#if KV_USED
static uint8_t app_wifi_ib_same(char *ssid, char *key){
	char wifi_ssid[40]={0};
	char wifi_key[40] = {0};
	int ssid_len = 40;
	int key_len = 40;
	if((HAL_Kv_Get("wifi_ssid", wifi_ssid, &ssid_len) == 0) && (strncmp(ssid,wifi_ssid,strlen(wifi_ssid)) == 0)){
	    if((HAL_Kv_Get("wifi_key", wifi_key, &key_len) == 0) &&(strncmp(key,wifi_key,strlen(wifi_key)) == 0)){
	 
			HAL_Printf("Same WiFi IB inputed\r\n");
			return 1;

	    }
	}
	return 0;


}
#endif
void app_process_recive_cmd(char *buff, uint8_t len){
  uint8_t ptr = 2;
  uint8_t i = 0;
  if(buff[0] == 'c'){//connect wifi
		char wifi_ssid[40]={0};
		char wifi_key[40] = {0};
		if(buff[1] == ' '){
			while(buff[ptr] != ' '){
				wifi_ssid[i++] = buff[ptr++];
			}
			ptr++;
			i=0;
			while(buff[ptr] != '\r' && (ptr<len)){
				wifi_key[i++] = buff[ptr++];
			}
#if KV_USED
			if(app_wifi_ib_same(wifi_ssid,wifi_key) == 0){
				HAL_Kv_Set("wifi_ssid", wifi_ssid, strlen(wifi_ssid), 0);
				HAL_Kv_Set("wifi_key", wifi_key, strlen(wifi_key), 0);
			}
#endif
			at_wifi_join(wifi_ssid,wifi_key);
                        HAL_Printf("join wifi:%s....\r\n",wifi_ssid);
		}

  }else if(buff[0] == 'f'){//factory new module
  	uint8_t value_invalid = 0xff;
#if KV_USED
  	HAL_Kv_Set("wifi_ssid", &value_invalid, 1, 0);
	HAL_Kv_Set("wifi_key", &value_invalid, 1, 0);
#endif
	at_wifi_factory_new();
        HAL_Printf("Factory wifi module....\r\n");
  }else{

  	HAL_Printf("Unknown command\r\n");

  }
  
}


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
	
    BOARD_BootClockRUN();
#if KV_USED
    flexspi_hyper_flash_init();
    kv_init();
#endif
    
    
    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	/* Init output LED GPIO. */
    GPIO_PinInit(BOARD_USER_LED_GPIO, BOARD_USER_LED_GPIO_PIN, &led_config);
    log_init();
    //linkkit_init();
	//linkkitcsdk_init();
    wm_init();
    vTaskStartScheduler();
    while(1);
}


