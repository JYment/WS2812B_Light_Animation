################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
stm_ws2812b.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm_ws2812b.list \

OBJCOPY_HEX += \
stm_ws2812b.hex \


# All Target
all: stm_ws2812b.elf secondary-outputs

# Tool invocations
stm_ws2812b.elf: $(OBJS) $(USER_OBJS) F:\2_WORK\Ext_Work\2019\Bluetooth_Speaker\firmware\STM32F103RETX_FLASH.ld
	arm-none-eabi-gcc -o "stm_ws2812b.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"F:\2_WORK\Ext_Work\2019\Bluetooth_Speaker\firmware\STM32F103RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm_ws2812b.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm_ws2812b.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm_ws2812b.list"
	@echo 'Finished building: $@'
	@echo ' '

stm_ws2812b.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "stm_ws2812b.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
