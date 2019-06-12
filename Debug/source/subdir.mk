################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/csdk.c \
../source/flexspi_hyper_flash_ops.c \
../source/fsl_lpuart_freertos.c \
../source/fsl_lpuart_log.c \
../source/linkkit_example_csdk.c \
../source/log.c \
../source/semihost_hardfault.c \
../source/washing_machine.c 

OBJS += \
./source/csdk.o \
./source/flexspi_hyper_flash_ops.o \
./source/fsl_lpuart_freertos.o \
./source/fsl_lpuart_log.o \
./source/linkkit_example_csdk.o \
./source/log.o \
./source/semihost_hardfault.o \
./source/washing_machine.o 

C_DEPS += \
./source/csdk.d \
./source/flexspi_hyper_flash_ops.d \
./source/fsl_lpuart_freertos.d \
./source/fsl_lpuart_log.d \
./source/linkkit_example_csdk.d \
./source/log.d \
./source/semihost_hardfault.d \
./source/washing_machine.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_ADVANCED_ENABLE -DMBEDTLS_CONFIG_FILE='"ksdk_mbedtls_config.h"' -DUSE_RTOS -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DCPU_MIMXRT1052DVL6B -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_RTOS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../source -I../CMSIS -I../amazon-freertos/freertos/portable -I../amazon-freertos/include -I../component/lists -I../component/serial_manager -I../component/uart -I../component/kv -I../component/kv/include -I../device -I../doc -I../drivers -I../src -I../startup -I../utilities -I../xip -I../csdk/eng -I../csdk/eng/atm -I../csdk/eng/dev_model -I../csdk/eng/dev_sign -I../csdk/eng/infra -I../csdk/eng/mqtt -I../csdk/eng/wrappers -I../csdk/eng/wrappers/external_libs -I../csdk/eng/coap_cloud -I../csdk/eng/coap_server -I../csdk/eng/coap_server/CoAPPacket -I../csdk/eng/coap_server/server -I../csdk/eng/dev_bind -I../csdk/eng/dev_bind/impl -I../csdk/eng/dev_bind/impl/os -I../csdk/eng/dev_bind/impl/awss_reset -I../mbedtls/include/mbedtls -I../mbedtls/include -I../mbedtls/port/ksdk -include../source/app_preinclude.h -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


