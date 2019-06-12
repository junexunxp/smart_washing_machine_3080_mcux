################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../amazon-freertos/freertos/event_groups.c \
../amazon-freertos/freertos/list.c \
../amazon-freertos/freertos/queue.c \
../amazon-freertos/freertos/stream_buffer.c \
../amazon-freertos/freertos/tasks.c \
../amazon-freertos/freertos/timers.c 

OBJS += \
./amazon-freertos/freertos/event_groups.o \
./amazon-freertos/freertos/list.o \
./amazon-freertos/freertos/queue.o \
./amazon-freertos/freertos/stream_buffer.o \
./amazon-freertos/freertos/tasks.o \
./amazon-freertos/freertos/timers.o 

C_DEPS += \
./amazon-freertos/freertos/event_groups.d \
./amazon-freertos/freertos/list.d \
./amazon-freertos/freertos/queue.d \
./amazon-freertos/freertos/stream_buffer.d \
./amazon-freertos/freertos/tasks.d \
./amazon-freertos/freertos/timers.d 


# Each subdirectory must supply rules for building sources it contributes
amazon-freertos/freertos/%.o: ../amazon-freertos/freertos/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_ADVANCED_ENABLE -DMBEDTLS_CONFIG_FILE='"ksdk_mbedtls_config.h"' -DUSE_RTOS -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DCPU_MIMXRT1052DVL6B -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_RTOS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../source -I../CMSIS -I../amazon-freertos/freertos/portable -I../amazon-freertos/include -I../component/lists -I../component/serial_manager -I../component/uart -I../component/kv -I../component/kv/include -I../device -I../doc -I../drivers -I../src -I../startup -I../utilities -I../xip -I../csdk/eng -I../csdk/eng/atm -I../csdk/eng/dev_model -I../csdk/eng/dev_sign -I../csdk/eng/infra -I../csdk/eng/mqtt -I../csdk/eng/wrappers -I../csdk/eng/wrappers/external_libs -I../csdk/eng/coap_cloud -I../csdk/eng/coap_server -I../csdk/eng/coap_server/CoAPPacket -I../csdk/eng/coap_server/server -I../csdk/eng/dev_bind -I../csdk/eng/dev_bind/impl -I../csdk/eng/dev_bind/impl/os -I../csdk/eng/dev_bind/impl/awss_reset -I../mbedtls/include/mbedtls -I../mbedtls/include -I../mbedtls/port/ksdk -include../source/app_preinclude.h -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


