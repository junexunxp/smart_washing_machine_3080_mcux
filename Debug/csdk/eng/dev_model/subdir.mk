################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csdk/eng/dev_model/dm_api.c \
../csdk/eng/dev_model/dm_client.c \
../csdk/eng/dev_model/dm_client_adapter.c \
../csdk/eng/dev_model/dm_cota.c \
../csdk/eng/dev_model/dm_fota.c \
../csdk/eng/dev_model/dm_ipc.c \
../csdk/eng/dev_model/dm_log_report.c \
../csdk/eng/dev_model/dm_manager.c \
../csdk/eng/dev_model/dm_message.c \
../csdk/eng/dev_model/dm_message_cache.c \
../csdk/eng/dev_model/dm_msg_process.c \
../csdk/eng/dev_model/dm_opt.c \
../csdk/eng/dev_model/dm_ota.c \
../csdk/eng/dev_model/dm_server.c \
../csdk/eng/dev_model/dm_server_adapter.c \
../csdk/eng/dev_model/dm_shadow.c \
../csdk/eng/dev_model/dm_tsl_alink.c \
../csdk/eng/dev_model/dm_utils.c \
../csdk/eng/dev_model/impl_gateway.c \
../csdk/eng/dev_model/impl_linkkit.c \
../csdk/eng/dev_model/impl_ntp.c \
../csdk/eng/dev_model/impl_solo.c \
../csdk/eng/dev_model/iotx_cm.c \
../csdk/eng/dev_model/iotx_cm_coap.c \
../csdk/eng/dev_model/iotx_cm_mqtt.c 

OBJS += \
./csdk/eng/dev_model/dm_api.o \
./csdk/eng/dev_model/dm_client.o \
./csdk/eng/dev_model/dm_client_adapter.o \
./csdk/eng/dev_model/dm_cota.o \
./csdk/eng/dev_model/dm_fota.o \
./csdk/eng/dev_model/dm_ipc.o \
./csdk/eng/dev_model/dm_log_report.o \
./csdk/eng/dev_model/dm_manager.o \
./csdk/eng/dev_model/dm_message.o \
./csdk/eng/dev_model/dm_message_cache.o \
./csdk/eng/dev_model/dm_msg_process.o \
./csdk/eng/dev_model/dm_opt.o \
./csdk/eng/dev_model/dm_ota.o \
./csdk/eng/dev_model/dm_server.o \
./csdk/eng/dev_model/dm_server_adapter.o \
./csdk/eng/dev_model/dm_shadow.o \
./csdk/eng/dev_model/dm_tsl_alink.o \
./csdk/eng/dev_model/dm_utils.o \
./csdk/eng/dev_model/impl_gateway.o \
./csdk/eng/dev_model/impl_linkkit.o \
./csdk/eng/dev_model/impl_ntp.o \
./csdk/eng/dev_model/impl_solo.o \
./csdk/eng/dev_model/iotx_cm.o \
./csdk/eng/dev_model/iotx_cm_coap.o \
./csdk/eng/dev_model/iotx_cm_mqtt.o 

C_DEPS += \
./csdk/eng/dev_model/dm_api.d \
./csdk/eng/dev_model/dm_client.d \
./csdk/eng/dev_model/dm_client_adapter.d \
./csdk/eng/dev_model/dm_cota.d \
./csdk/eng/dev_model/dm_fota.d \
./csdk/eng/dev_model/dm_ipc.d \
./csdk/eng/dev_model/dm_log_report.d \
./csdk/eng/dev_model/dm_manager.d \
./csdk/eng/dev_model/dm_message.d \
./csdk/eng/dev_model/dm_message_cache.d \
./csdk/eng/dev_model/dm_msg_process.d \
./csdk/eng/dev_model/dm_opt.d \
./csdk/eng/dev_model/dm_ota.d \
./csdk/eng/dev_model/dm_server.d \
./csdk/eng/dev_model/dm_server_adapter.d \
./csdk/eng/dev_model/dm_shadow.d \
./csdk/eng/dev_model/dm_tsl_alink.d \
./csdk/eng/dev_model/dm_utils.d \
./csdk/eng/dev_model/impl_gateway.d \
./csdk/eng/dev_model/impl_linkkit.d \
./csdk/eng/dev_model/impl_ntp.d \
./csdk/eng/dev_model/impl_solo.d \
./csdk/eng/dev_model/iotx_cm.d \
./csdk/eng/dev_model/iotx_cm_coap.d \
./csdk/eng/dev_model/iotx_cm_mqtt.d 


# Each subdirectory must supply rules for building sources it contributes
csdk/eng/dev_model/%.o: ../csdk/eng/dev_model/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_ADVANCED_ENABLE -DMBEDTLS_CONFIG_FILE='"ksdk_mbedtls_config.h"' -DUSE_RTOS -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DCPU_MIMXRT1052DVL6B -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_RTOS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../source -I../CMSIS -I../amazon-freertos/freertos/portable -I../amazon-freertos/include -I../component/lists -I../component/serial_manager -I../component/uart -I../component/kv -I../component/kv/include -I../device -I../doc -I../drivers -I../src -I../startup -I../utilities -I../xip -I../csdk/eng -I../csdk/eng/atm -I../csdk/eng/dev_model -I../csdk/eng/dev_sign -I../csdk/eng/infra -I../csdk/eng/mqtt -I../csdk/eng/wrappers -I../csdk/eng/wrappers/external_libs -I../csdk/eng/coap_cloud -I../csdk/eng/coap_server -I../csdk/eng/coap_server/CoAPPacket -I../csdk/eng/coap_server/server -I../csdk/eng/dev_bind -I../csdk/eng/dev_bind/impl -I../csdk/eng/dev_bind/impl/os -I../csdk/eng/dev_bind/impl/awss_reset -I../mbedtls/include/mbedtls -I../mbedtls/include -I../mbedtls/port/ksdk -include../source/app_preinclude.h -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


