################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xip/subdir.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include mbedtls/port/ksdk/subdir.mk
-include mbedtls/library/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include csdk/examples/subdir.mk
-include csdk/eng/wrappers/tls/subdir.mk
-include csdk/eng/wrappers/external_libs/subdir.mk
-include csdk/eng/wrappers/at_tcp/subdir.mk
-include csdk/eng/wrappers/subdir.mk
-include csdk/eng/mqtt/subdir.mk
-include csdk/eng/infra/subdir.mk
-include csdk/eng/dev_sign/subdir.mk
-include csdk/eng/dev_model/subdir.mk
-include csdk/eng/dev_bind/impl/os/subdir.mk
-include csdk/eng/dev_bind/impl/awss_reset/subdir.mk
-include csdk/eng/dev_bind/impl/subdir.mk
-include csdk/eng/coap_server/server/subdir.mk
-include csdk/eng/coap_server/CoAPPacket/subdir.mk
-include csdk/eng/certs/subdir.mk
-include csdk/eng/atm/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include component/kv/subdir.mk
-include board/subdir.mk
-include amazon-freertos/freertos/portable/subdir.mk
-include amazon-freertos/freertos/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: smart_washing_machine_3080.axf

# Tool invocations
smart_washing_machine_3080.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="smart_washing_machine_3080.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -T smart_washing_machine_3080_freertos_hello_Debug.ld -o "smart_washing_machine_3080.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) smart_washing_machine_3080.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "smart_washing_machine_3080.axf"; # arm-none-eabi-objcopy -v -O binary "smart_washing_machine_3080.axf" "smart_washing_machine_3080.bin" ; # checksum -p MIMXRT1052xxxxB -d "smart_washing_machine_3080.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
