/**
 * NOTE:
 *
 * HAL_TCP_xxx API reference implementation: wrappers/os/ubuntu/HAL_TCP_linux.c
 *
 */
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_compat.h"
#include "wrappers_defs.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "at_wrapper.h"
#include "mimxrt1052.h"
#include "fsl_trng.h"
#include "fsl_clock.h"
#include "fsl_lpuart.h"


unsigned char srandom_inited = 0;
#ifndef SECURITY_MATERIAL_APP
#define PRODUCT_KEY_D      "a1I5Y6v8HwT"
#define PRODUCT_SECRET_D   "S56FG57Rqjr24CHo"
#define DEVICE_NAME_D      "smart_wm_test1"
#define DEVICE_SECRET_D    "3qLxqnrCCxhEVJUJjq88jEYkPwi2CZCr"
#define FIRMWARE_VER_D	 "1.1.1"
#endif

#ifndef EIO
#define EIO 5
#endif

#define UART_TX_INT_THRESHOLD 1
static volatile uint32_t sys_tick_cnt;
static volatile uint8_t wrapped = 0;
static const uint32_t s_uartBaseAddrs[8] = {(uint32_t)LPUART1_BASE, (uint32_t)LPUART2_BASE,
(uint32_t)LPUART3_BASE, (uint32_t)LPUART4_BASE, (uint32_t)LPUART5_BASE, (uint32_t)LPUART6_BASE, (uint32_t)LPUART7_BASE, 
(uint32_t)LPUART8_BASE};
static lpuart_handle_t s_handle[9];


volatile static uint32_t uart_status;
volatile static uint32_t uart_rxstatus;
TaskHandle_t isr_uart_handle;
QueueHandle_t uart_txwait_sem,uart_rxwait_sem,check_st_sem;

static uint8_t seam_wait_tx,seam_wait_rx;
/* Global Variable for VFS DEBUG Output */
static void uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_LPUART_TxIdle == status)
    {
        uart_status = kStatus_LPUART_TxIdle;

    }

    if (kStatus_LPUART_RxIdle == status)
    {
        uart_rxstatus = kStatus_LPUART_RxIdle;

    }
}


int32_t HAL_Uart_rxWait(uint32_t timeout){
#if 1
  TickType_t xdelay = timeout/portTICK_PERIOD_MS;
  volatile uint32_t tick_prev = xTaskGetTickCount();
  while(uart_rxstatus == kStatus_LPUART_RxBusy){
  		if(xTaskGetTickCount() - tick_prev  > xdelay){

			return 1;

  		}
  }
	return 0;
#else
	while(uart_rxstatus == kStatus_LPUART_RxBusy){
		seam_wait_rx = 1;
		xSemaphoreGive(check_st_sem);
		HAL_SemaphoreWait(uart_rxwait_sem,timeout);
		seam_wait_rx = 0;
		if(uart_rxstatus == kStatus_LPUART_RxBusy)
			return -1;
	}
	return 0;
#endif
}


int32_t HAL_Uart_txWait(uint32_t timeout){
#if 1
    TickType_t xdelay = timeout/portTICK_PERIOD_MS;
  volatile uint32_t tick_prev = xTaskGetTickCount();
  while(uart_status == kStatus_LPUART_TxBusy){
      if(xTaskGetTickCount() - tick_prev  > xdelay){

			return 1;

  		}
  }
	return 0;
#else
	if(uart_status == kStatus_LPUART_TxBusy){
		seam_wait_tx = 1;
		xSemaphoreGive(check_st_sem);
		HAL_SemaphoreWait(uart_txwait_sem,timeout);
		
		seam_wait_tx = 0;
		if(uart_status == kStatus_LPUART_TxBusy)
			return -1;
	}
	return 0;
#endif

}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Deinit(uart_dev_t *uart)
{
	LPUART_Deinit((LPUART_Type *)s_uartBaseAddrs[uart->port]);
	HAL_SemaphoreDestroy(uart_rxwait_sem);
	HAL_SemaphoreDestroy(uart_txwait_sem);
	HAL_SemaphoreDestroy(check_st_sem);
	
	return (int32_t)0;
}

void uart2_interrupt_handle_task( void *pvParameters )
{
    for( ;; )
    {
        if( seam_wait_tx || seam_wait_rx )
        {
        	if((uart_status == kStatus_LPUART_TxIdle)&&seam_wait_tx){
				seam_wait_tx = 0;
				xSemaphoreGive(uart_txwait_sem);

        	}
			if((uart_rxstatus == kStatus_LPUART_RxIdle)&&seam_wait_rx){
				seam_wait_rx = 0;
				xSemaphoreGive(uart_rxwait_sem);
        	}

        }else{
			HAL_SemaphoreWait(check_st_sem,5000);
        }
    }
}


/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
#define USE_LPUART_RTOS 1
#if USE_LPUART_RTOS
#include "fsl_lpuart_freertos.h"
lpuart_rtos_handle_t handle;

struct _lpuart_handle t_handle;
uint8_t background_buffer[512];
lpuart_rtos_config_t lpuart_config = {
    .baudrate    = 115200,
    .parity      = kLPUART_ParityDisabled,
    .stopbits    = kLPUART_OneStopBit,
    .buffer      = background_buffer,
    .buffer_size = sizeof(background_buffer),
};
#endif
int32_t HAL_AT_Uart_Init(uart_dev_t *uart)
{
#if USE_LPUART_RTOS
  lpuart_config.srcclk = BOARD_DebugConsoleSrcFreq();
  lpuart_config.base   = (LPUART_Type *)s_uartBaseAddrs[uart->port];
  NVIC_SetPriority(LPUART3_IRQn, 5);
  LPUART_RTOS_Init(&handle, &t_handle, &lpuart_config);
#else
	lpuart_config_t config = {0};
	status_t status;
	uint8_t *addr;
	size_t size;
	
	LPUART_GetDefaultConfig(&config);
	config.baudRate_Bps = uart->config.baud_rate;
	config.enableTx = true;
	config.enableRx = true;

	switch (uart->config.parity)
	{
		case NO_PARITY:
			config.parityMode = kLPUART_ParityDisabled;
			break;
		case ODD_PARITY:
			config.parityMode = kLPUART_ParityOdd;
			break;
		case EVEN_PARITY:
			config.parityMode = kLPUART_ParityEven;
			break;
		default:
			return EIO;
	}

	switch (uart->config.data_width)
	{
#if defined(FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT) && FSL_FEATURE_LPUART_HAS_7BIT_DATA_SUPPORT
		case DATA_WIDTH_7BIT:
			config.dataBitsCount = kLPUART_SevenDataBits;
			break;
#endif
		case DATA_WIDTH_8BIT:
			config.dataBitsCount = kLPUART_EightDataBits;
			break;
		default:
			return EIO;
	}

	switch (uart->config.stop_bits)
	{
		case STOP_BITS_1:
			config.stopBitCount = kLPUART_OneStopBit;
			break;
		case STOP_BITS_2:
			config.stopBitCount = kLPUART_TwoStopBit;
			break;
		default:
			return EIO;
	}
	
	/* TODO: Need to have array to save clock source for all uart instances */
	uint32_t freq = 0;
	freq = BOARD_DebugConsoleSrcFreq();
	status = LPUART_Init((LPUART_Type *)s_uartBaseAddrs[uart->port], &config, freq);
	
	if (kStatus_Success != status)
		return EIO;
	

	/* Create handle for LPUART */
	LPUART_TransferCreateHandle((LPUART_Type *)s_uartBaseAddrs[uart->port], &s_handle[uart->port],
								uart_callback, uart);

	if (uart->port == 2){

		size = 512;
		addr = HAL_Malloc(size);
		if (addr != NULL){
		    LPUART_TransferStartRingBuffer((LPUART_Type *)s_uartBaseAddrs[uart->port], &s_handle[uart->port],
		                                    addr, size);
		}else{
		    return -1;
		}
	}
        
#if 1
        
#else
	xTaskCreate(
                  uart2_interrupt_handle_task,       /* Function that implements the task. */
                  "u2_isr_handle",          /* Text name for the task. */
                  256,      /* Stack size in words, not bytes. */
                  NULL,    /* Parameter passed into the task. */
                  (0),/* Priority at which the task is created. */
                  &isr_uart_handle);      /* Used to pass out the created task's handle. */
	uart_rxwait_sem = HAL_SemaphoreCreate();
	uart_txwait_sem = HAL_SemaphoreCreate();
	check_st_sem = HAL_SemaphoreCreate();
        
#endif
#endif
	return 0;
}


/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Recv(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
  #if USE_LPUART_RTOS
  int ret = LPUART_RTOS_Receive(&handle, data, expect_size, recv_size,timeout/portTICK_PERIOD_MS);
  if(ret != 0){
	ret = EIO;
  }
  return ret;
#else
	if(uart_rxstatus == kStatus_LPUART_RxBusy){
			return EIO;
		}

	lpuart_transfer_t xfer;
    int32_t ret = 0;
    LPUART_Type *uartBase = (LPUART_Type *)s_uartBaseAddrs[uart->port];
    lpuart_handle_t *handle = &s_handle[uart->port];

    if (timeout == 0)
    {
        timeout = 1000;
    }

    xfer.data = (uint8_t *)data;
    xfer.dataSize = expect_size;
	uart_rxstatus = kStatus_LPUART_RxBusy;
    if (kStatus_Success != LPUART_TransferReceiveNonBlocking(uartBase, handle, &xfer, recv_size))
    {
		uart_rxstatus = kStatus_LPUART_RxIdle;
        return EIO;
    }
	



    if (HAL_Uart_rxWait(timeout) != 0)
    {
        LPUART_TransferAbortReceive(uartBase, handle);
		uart_rxstatus = kStatus_LPUART_RxIdle;

        *recv_size = 0;

        ret = EIO;
    }
    else
    {
        *recv_size = expect_size;
    }
  return ret;
#endif
   
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t HAL_AT_Uart_Send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
  #if USE_LPUART_RTOS
  if (timeout == 0)
  {
	  timeout = 1000;
  }
  int ret = 0;
  ret = LPUART_RTOS_Send(&handle, (uint8_t *)data, size,timeout/portTICK_PERIOD_MS);
  if(ret != 0){
	

	ret = EIO;

  }
  return ret;
#else
	if(uart_status == kStatus_LPUART_TxBusy){
		return EIO;
	}
	lpuart_transfer_t xfer;
    int32_t ret = 0;
    LPUART_Type *uartBase = (LPUART_Type *)s_uartBaseAddrs[uart->port];
    lpuart_handle_t *handle = &s_handle[uart->port];

    if (timeout == 0)
    {
        timeout = 1000;
    }



    if (size < UART_TX_INT_THRESHOLD)
    {
        LPUART_WriteBlocking(uartBase, (uint8_t *)data, size);
    }
    else
    {
        xfer.data = (uint8_t *)data;
        
        xfer.dataSize = size;
		uart_status = kStatus_LPUART_TxBusy;
        LPUART_TransferSendNonBlocking(uartBase, handle, &xfer);
        /* Wait for transfer finish */
       
        if (HAL_Uart_txWait(timeout) != 0)
        {
            LPUART_TransferAbortSend(uartBase, handle);
			uart_status = kStatus_LPUART_TxIdle;

            ret = EIO;
        }
    }


    return ret;
#endif
}
/**
 * @brief Deallocate memory block
 *
 * @param[in] ptr @n Pointer to a memory block previously allocated with platform_malloc.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_Free(void *ptr)
{
	vPortFree(ptr);
}


/**
 * @brief Get device name from user's system persistent storage
 *
 * @param [ou] device_name: array to store device name, max length is IOTX_DEVICE_NAME_LEN
 * @return the actual length of device name
 */
int HAL_GetDeviceName(char device_name[IOTX_DEVICE_NAME_LEN + 1])
{

	if(strlen(DEVICE_NAME_D) <= IOTX_DEVICE_NAME_LEN){
                memset(device_name, 0x0, IOTX_DEVICE_NAME_LEN);
		strncpy(device_name, DEVICE_NAME_D, strlen(DEVICE_NAME_D));
		return (int)strlen(device_name);
	}else{

		return -1;
	}
}


/**
 * @brief Get device secret from user's system persistent storage
 *
 * @param [ou] device_secret: array to store device secret, max length is IOTX_DEVICE_SECRET_LEN
 * @return the actual length of device secret
 */
int HAL_GetDeviceSecret(char device_secret[IOTX_DEVICE_SECRET_LEN + 1])
{
	if(strlen(DEVICE_SECRET_D) <= IOTX_DEVICE_SECRET_LEN){
                memset(device_secret, 0x0, IOTX_DEVICE_SECRET_LEN);
		strncpy(device_secret, DEVICE_SECRET_D, strlen(DEVICE_SECRET_D));
		return (int)strlen(DEVICE_SECRET_D);
	}else{

		return -1;
	}
}


/**
 * @brief Get firmware version
 *
 * @param [ou] version: array to store firmware version, max length is IOTX_FIRMWARE_VER_LEN
 * @return the actual length of firmware version
 */
int HAL_GetFirmwareVersion(char *version)
{
	if(strlen(FIRMWARE_VER_D) <= IOTX_FIRMWARE_VER_LEN){
                memset(version, 0x0, IOTX_FIRMWARE_VER_LEN);
		strncpy(version, FIRMWARE_VER_D, strlen(FIRMWARE_VER_D));
		version[strlen(FIRMWARE_VER_D)] = '\0';
		return (int)strlen(version);
	}else{
		return -1;
	}
}


/**
 * @brief Get product key from user's system persistent storage
 *
 * @param [ou] product_key: array to store product key, max length is IOTX_PRODUCT_KEY_LEN
 * @return  the actual length of product key
 */
int HAL_GetProductKey(char product_key[IOTX_PRODUCT_KEY_LEN + 1])
{
	if(strlen(PRODUCT_KEY_D) <= IOTX_PRODUCT_KEY_LEN){
                memset(product_key, 0x0, IOTX_PRODUCT_KEY_LEN);
		strncpy(product_key, PRODUCT_KEY_D, strlen(PRODUCT_KEY_D));

		return (int)strlen(PRODUCT_KEY_D);
	}else{
		return -1;
	}
}


int HAL_GetProductSecret(char product_secret[IOTX_PRODUCT_SECRET_LEN + 1])
{
	if(sizeof(PRODUCT_SECRET_D) <= IOTX_PRODUCT_SECRET_LEN){
                memset(product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN);
		strncpy(product_secret, PRODUCT_SECRET_D, strlen(PRODUCT_SECRET_D));

		return (int)strlen(PRODUCT_SECRET_D);
	}else{
		return -1;
	}
}

int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
    return kv_item_get(key, val, buffer_len);
}

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{  
    return kv_item_set(key, val, len);
}
/**
 * @brief Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * @param [in] size @n specify block size in bytes.
 * @return A pointer to the beginning of the block.
 * @see None.
 * @note Block value is indeterminate.
 */
void *HAL_Malloc(uint32_t size)
{
	return ((void *)pvPortMalloc(size));
}


/**
 * @brief Create a mutex.
 *
 * @retval NULL : Initialize mutex failed.
 * @retval NOT_NULL : The mutex handle.
 * @see None.
 * @note None.
 */
void *HAL_MutexCreate(void)
{
      QueueHandle_t sem;

    sem = (QueueHandle_t)xSemaphoreCreateMutex();
    if (0 == sem) {
      return NULL;
    }

    return sem;
}


/**
 * @brief Destroy the specified mutex object, it will release related resource.
 *
 * @param [in] mutex @n The specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexDestroy(void *mutex)
{
	QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }
    sem = (QueueHandle_t)mutex;
    vSemaphoreDelete(sem);
}


/**
 * @brief Waits until the specified mutex is in the signaled state.
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexLock(void *mutex)
{
	BaseType_t ret;
    QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }

    sem = (QueueHandle_t)mutex;
    ret = xSemaphoreTake(sem, 0xffffffff);
    while (pdPASS != ret) {
        ret = xSemaphoreTake(sem, 0xffffffff);
    }
}


/**
 * @brief Releases ownership of the specified mutex object..
 *
 * @param [in] mutex @n the specified mutex.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_MutexUnlock(void *mutex)
{
	QueueHandle_t sem;
    if (mutex == NULL) {
        return;
    }
    sem = (QueueHandle_t)mutex;
    (void)xSemaphoreGive(sem);
}


/**
 * @brief Writes formatted data to stream.
 *
 * @param [in] fmt: @n String that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return None.
 * @see None.
 * @note None.
 */
 #if 0
void HAL_Printf(const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        format_printf(fmt,args);
        va_end(args);
	return;
}
#endif

uint32_t HAL_Random(uint32_t region)
{
    uint32_t rand = 0xffffffff;
    
    TRNG_GetRandomData(TRNG, (void *)&rand, 1);
        
	return (rand%region);
}
/**
 * @brief Sleep thread itself.
 *
 * @param [in] ms @n the time interval for which execution is to be suspended, in milliseconds.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SleepMs(uint32_t ms)
{
	const TickType_t xdelay = ms/portTICK_PERIOD_MS;
	vTaskDelay(xdelay);
    //uint32_t current_tick = xTaskGetTickCount();
	//uint32_t anchor = current_tick + ms;
     //   uint32_t loopb = 0xffffffff;
      //  while(loopb--);
	//while((anchor - current_tick>0) && (anchor - current_tick < 0xffffff)){
         // current_tick = xTaskGetTickCount();
        //}
}


/**
 * @brief Writes formatted data to string.
 *
 * @param [out] str: @n String that holds written text.
 * @param [in] len: @n Maximum length of character will be written
 * @param [in] fmt: @n Format that contains the text to be written, it can optionally contain embedded format specifiers
     that specifies how subsequent arguments are converted for output.
 * @param [in] ...: @n the variable argument list, for formatted and inserted in the resulting string replacing their respective specifiers.
 * @return bytes of character successfully written into string.
 * @see None.
 * @note None.
 */
int HAL_Snprintf(char *str, const int len, const char *fmt, ...)
{
	va_list args;
	int rc;

	va_start(args, fmt);
	rc = vsnprintf(str, len, fmt, args);
	va_end(args);
	return rc;


}


void HAL_Srandom(uint32_t seed)
{
	if (!srandom_inited){
	
		trng_config_t trngConfig;
		/* Initialize TRNG configuration structure to default.*/
		/*
		 * trngConfig.lock = TRNG_USER_CONFIG_DEFAULT_LOCK;
		 * trngConfig.clockMode = kTRNG_ClockModeRingOscillator;
		 * trngConfig.ringOscDiv = TRNG_USER_CONFIG_DEFAULT_OSC_DIV;
		 * trngConfig.sampleMode = kTRNG_SampleModeRaw;
		 * trngConfig.entropyDelay = TRNG_USER_CONFIG_DEFAULT_ENTROPY_DELAY;
		 * trngConfig.sampleSize = TRNG_USER_CONFIG_DEFAULT_SAMPLE_SIZE;
		 * trngConfig.sparseBitLimit = TRNG_USER_CONFIG_DEFAULT_SPARSE_BIT_LIMIT;
		 * trngConfig.retryCount = TRNG_USER_CONFIG_DEFAULT_RETRY_COUNT;
		 * trngConfig.longRunMaxLimit = TRNG_USER_CONFIG_DEFAULT_RUN_MAX_LIMIT;
		 * trngConfig.monobitLimit.maximum = TRNG_USER_CONFIG_DEFAULT_MONOBIT_MAXIMUM;
		 * trngConfig.monobitLimit.minimum = TRNG_USER_CONFIG_DEFAULT_MONOBIT_MINIMUM;
		 * trngConfig.runBit1Limit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MAXIMUM;
		 * trngConfig.runBit1Limit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT1_MINIMUM;
		 * trngConfig.runBit2Limit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MAXIMUM;
		 * trngConfig.runBit2Limit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT2_MINIMUM;
		 * trngConfig.runBit3Limit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MAXIMUM;
		 * trngConfig.runBit3Limit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT3_MINIMUM;
		 * trngConfig.runBit4Limit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MAXIMUM;
		 * trngConfig.runBit4Limit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT4_MINIMUM;
		 * trngConfig.runBit5Limit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MAXIMUM;
		 * trngConfig.runBit5Limit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT5_MINIMUM;
		 * trngConfig.runBit6PlusLimit.maximum = TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MAXIMUM;
		 * trngConfig.runBit6PlusLimit.minimum = TRNG_USER_CONFIG_DEFAULT_RUNBIT6PLUS_MINIMUM;
		 * trngConfig.pokerLimit.maximum = TRNG_USER_CONFIG_DEFAULT_POKER_MAXIMUM;
		 * trngConfig.pokerLimit.minimum = TRNG_USER_CONFIG_DEFAULT_POKER_MINIMUM;
		 * trngConfig.frequencyCountLimit.maximum = TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MAXIMUM;
		 * trngConfig.frequencyCountLimit.minimum = TRNG_USER_CONFIG_DEFAULT_FREQUENCY_MINIMUM;
		 */
		TRNG_GetDefaultConfig(&trngConfig);
		/* Set sample mode of the TRNG ring oscillator to Von Neumann, for better random data.*/
		trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;
		/* Initialize TRNG */
		TRNG_Init(TRNG, &trngConfig);
	}

	return;
}

/**
 * @brief   create a semaphore
 *
 * @return semaphore handle.
 * @see None.
 * @note The recommended value of maximum count of the semaphore is 255.
 */
void *HAL_SemaphoreCreate(void)
{
	QueueHandle_t sem = 0;
    sem = (QueueHandle_t)xSemaphoreCreateCounting(10, 0);
    if (0 == sem) {
        return NULL;
    }

    return sem;
}


/**
 * @brief   destory a semaphore
 *
 * @param[in] sem @n the specified sem.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphoreDestroy(void *sem)
{
	QueueHandle_t queue;

    if (sem == NULL) {
        return;
    }
    queue = (QueueHandle_t)sem;

    vSemaphoreDelete(queue);
}


/**
 * @brief   signal thread wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @return None.
 * @see None.
 * @note None.
 */
void HAL_SemaphorePost(void *sem)
{
	QueueHandle_t queue;
    if (sem == NULL) {
        return;
    }
    queue = (QueueHandle_t)sem;
    (void)xSemaphoreGive(queue);
}
/**
 * @brief   wait on a semaphore
 *
 * @param[in] sem @n the specified semaphore.
 * @param[in] timeout_ms @n timeout interval in millisecond.
     If timeout_ms is PLATFORM_WAIT_INFINITE, the function will return only when the semaphore is signaled.
 * @return
   @verbatim
   =  0: The state of the specified object is signaled.
   =  -1: The time-out interval elapsed, and the object's state is nonsignaled.
   @endverbatim
 * @see None.
 * @note None.
 */
int HAL_SemaphoreWait(void *sem, uint32_t timeout_ms)
{
	BaseType_t ret = 0;
    QueueHandle_t queue;
    if (sem == NULL) {
        return -1;
    }

    queue = (QueueHandle_t)sem;
    ret = xSemaphoreTake(queue, timeout_ms/portTICK_PERIOD_MS);
    if (pdPASS != ret) {
        return -1;
    }
    return 0;
}

/**
 * @brief  create a thread
 *
 * @param[out] thread_handle @n The new thread handle, memory allocated before thread created and return it, free it after thread joined or exit.
 * @param[in] start_routine @n A pointer to the application-defined function to be executed by the thread.
        This pointer represents the starting address of the thread.
 * @param[in] arg @n A pointer to a variable to be passed to the start_routine.
 * @param[in] hal_os_thread_param @n A pointer to stack params.
 * @param[out] stack_used @n if platform used stack buffer, set stack_used to 1, otherwise set it to 0.
 * @return
   @verbatim
     = 0: on success.
     = -1: error occur.
   @endverbatim
 * @see None.
 * @note None.
 */
 #ifndef DEFAULT_THREAD_NAME
 #define DEFAULT_THREAD_NAME "wrapper_"
 #define TASK_STACK_ALIGN_SIZE 4u
 #endif
 
int HAL_ThreadCreate(
            void **thread_handle,
            void *(*work_routine)(void *),
            void *arg,
            hal_os_thread_param_t *hal_os_thread_param,
            int *stack_used)
{
  

	char *name;
    size_t stacksize;

    if (thread_handle == NULL) {
        return -1;
    }

    if (work_routine == NULL) {
        return -1;
    }

    if (hal_os_thread_param == NULL) {
        return -1;
    }
    if (stack_used == NULL) {
        return -1;
    }

    if (stack_used != NULL) {
        *stack_used = 0;
    }

    if (!hal_os_thread_param->name) {
        name = DEFAULT_THREAD_NAME;
    } else {
        name = hal_os_thread_param->name;
    }

    if (hal_os_thread_param->stack_size == 0) {
        stacksize = configMINIMAL_STACK_SIZE;
    } else {
        stacksize = hal_os_thread_param->stack_size;
    }

 
    stacksize = (stacksize + TASK_STACK_ALIGN_SIZE - 1) / TASK_STACK_ALIGN_SIZE;
	static TaskHandle_t xtask = NULL;
    BaseType_t ret = xTaskCreate((TaskFunction_t )(work_routine), name,stacksize,arg,hal_os_thread_param->priority,&xtask);
    if (NULL == ret) {
        return -1;
    }
    *thread_handle = (void *)xtask;

    return 0;
}
void HAL_ThreadDelete(void *thread_handle)
{
	vTaskDelete((TaskHandle_t )thread_handle);
}


void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
	TimerHandle_t thandle = xTimerCreate(name, 1000, pdFALSE, user_data, (TimerCallbackFunction_t)func);
	
	return (void *)thandle;
}


int HAL_Timer_Delete(void *timer)
{
	return (int )xTimerDelete((TimerHandle_t ) timer, 0);
}


int HAL_Timer_Start(void *timer, int ms)
{
	BaseType_t ret = xTimerStart((TimerHandle_t ) timer,pdMS_TO_TICKS(ms));
	return (int )ret;
}


int HAL_Timer_Stop(void *timer)
{
	BaseType_t ret = xTimerStop((TimerHandle_t ) timer, 0);
	return (int )ret;
}



/**
 * @brief Retrieves the number of milliseconds that have elapsed since the system was boot.
 *
 * @return the number of milliseconds.
 * @see None.
 * @note None.
 */
uint64_t HAL_UptimeMs(void)
{
	return (uint64_t)xTaskGetTickCount();
}
/*
void SysTick_Handler(void)
{
    sys_tick_cnt++;
	if(!sys_tick_cnt){
		wrapped = 1;
	}
	if(sys_tick_cnt > 20){
		wrapped = 0;

	}
}
*/

void HAL_timerInit(void ){
  SysTick_Config(CLOCK_GetFreq(kCLOCK_CoreSysClk) / 1000U);
}

int HAL_Vsnprintf(char *str, const int len, const char *format, va_list ap)
{
	return 1;
}
