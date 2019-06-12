################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../csdk/eng/infra/infra_cjson.c \
../csdk/eng/infra/infra_compat.c \
../csdk/eng/infra/infra_defs.c \
../csdk/eng/infra/infra_json_parser.c \
../csdk/eng/infra/infra_log.c \
../csdk/eng/infra/infra_md5.c \
../csdk/eng/infra/infra_net.c \
../csdk/eng/infra/infra_prt_nwk_payload.c \
../csdk/eng/infra/infra_report.c \
../csdk/eng/infra/infra_sha1.c \
../csdk/eng/infra/infra_sha256.c \
../csdk/eng/infra/infra_string.c \
../csdk/eng/infra/infra_timer.c 

OBJS += \
./csdk/eng/infra/infra_cjson.o \
./csdk/eng/infra/infra_compat.o \
./csdk/eng/infra/infra_defs.o \
./csdk/eng/infra/infra_json_parser.o \
./csdk/eng/infra/infra_log.o \
./csdk/eng/infra/infra_md5.o \
./csdk/eng/infra/infra_net.o \
./csdk/eng/infra/infra_prt_nwk_payload.o \
./csdk/eng/infra/infra_report.o \
./csdk/eng/infra/infra_sha1.o \
./csdk/eng/infra/infra_sha256.o \
./csdk/eng/infra/infra_string.o \
./csdk/eng/infra/infra_timer.o 

C_DEPS += \
./csdk/eng/infra/infra_cjson.d \
./csdk/eng/infra/infra_compat.d \
./csdk/eng/infra/infra_defs.d \
./csdk/eng/infra/infra_json_parser.d \
./csdk/eng/infra/infra_log.d \
./csdk/eng/infra/infra_md5.d \
./csdk/eng/infra/infra_net.d \
./csdk/eng/infra/infra_prt_nwk_payload.d \
./csdk/eng/infra/infra_report.d \
./csdk/eng/infra/infra_sha1.d \
./csdk/eng/infra/infra_sha256.d \
./csdk/eng/infra/infra_string.d \
./csdk/eng/infra/infra_timer.d 


# Each subdirectory must supply rules for building sources it contributes
csdk/eng/infra/%.o: ../csdk/eng/infra/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_ADVANCED_ENABLE -DMBEDTLS_CONFIG_FILE='"ksdk_mbedtls_config.h"' -DUSE_RTOS -DCPU_MIMXRT1052CVL5B -DCPU_MIMXRT1052CVL5B_cm7 -DCPU_MIMXRT1052DVL6B -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_RTOS_FREE_RTOS -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../source -I../CMSIS -I../amazon-freertos/freertos/portable -I../amazon-freertos/include -I../component/lists -I../component/serial_manager -I../component/uart -I../component/kv -I../component/kv/include -I../device -I../doc -I../drivers -I../src -I../startup -I../utilities -I../xip -I../csdk/eng -I../csdk/eng/atm -I../csdk/eng/dev_model -I../csdk/eng/dev_sign -I../csdk/eng/infra -I../csdk/eng/mqtt -I../csdk/eng/wrappers -I../csdk/eng/wrappers/external_libs -I../csdk/eng/coap_cloud -I../csdk/eng/coap_server -I../csdk/eng/coap_server/CoAPPacket -I../csdk/eng/coap_server/server -I../csdk/eng/dev_bind -I../csdk/eng/dev_bind/impl -I../csdk/eng/dev_bind/impl/os -I../csdk/eng/dev_bind/impl/awss_reset -I../mbedtls/include/mbedtls -I../mbedtls/include -I../mbedtls/port/ksdk -include../source/app_preinclude.h -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


