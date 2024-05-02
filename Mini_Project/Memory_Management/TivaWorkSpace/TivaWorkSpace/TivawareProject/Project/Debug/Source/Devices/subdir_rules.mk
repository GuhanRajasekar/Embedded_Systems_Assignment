################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Source/Devices/ADC.o: /home/tessin/Tessin/TivawareProject/Source/Devices/ADC.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/Clock.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/Clock.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/DigitalInputPort.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/DigitalInputPort.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/DigitalOutputPort.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/DigitalOutputPort.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/KeyBoard.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/KeyBoard.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/LED.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/LED.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/MatrixKBD.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/MatrixKBD.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/PWM.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/PWM.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/SysTimer.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/SysTimer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/Timer.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/Timer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

Source/Devices/UART.o: /home/tessin/Tessin/TivaWorkSpace/TivawareProject/Source/Devices/UART.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/home/tessin/ti/ccsv6/tools/compiler/gcc-arm-none-eabi-4_9-2015q3/arm-none-eabi/include" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Devices" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/Genlib" -I"/home/tessin/Tessin/TivaWorkSpace/TivawareProject/Headers/GenApp" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/inc" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156/driverlib" -I"/home/tessin/ti/TivaWare_C_Series-2.1.3.156" -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MD -std=c99 -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o"$@" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


