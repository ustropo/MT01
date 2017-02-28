################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
..\r_bsp/mcu/rx63n/cpu.c \
..\r_bsp/mcu/rx63n/locking.c \
..\r_bsp/mcu/rx63n/mcu_init.c \
..\r_bsp/mcu/rx63n/mcu_interrupts.c \
..\r_bsp/mcu/rx63n/mcu_locks.c 

C_DEPS += \
./r_bsp/mcu/rx63n/cpu.d \
./r_bsp/mcu/rx63n/locking.d \
./r_bsp/mcu/rx63n/mcu_init.d \
./r_bsp/mcu/rx63n/mcu_interrupts.d \
./r_bsp/mcu/rx63n/mcu_locks.d 

OBJS += \
./r_bsp/mcu/rx63n/cpu.obj \
./r_bsp/mcu/rx63n/locking.obj \
./r_bsp/mcu/rx63n/mcu_init.obj \
./r_bsp/mcu/rx63n/mcu_interrupts.obj \
./r_bsp/mcu/rx63n/mcu_locks.obj 


# Each subdirectory must supply rules for building sources it contributes
r_bsp/mcu/rx63n/%.obj: ../r_bsp/mcu/rx63n/%.c
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	ccrx  -MM -MP -output=dep="$(@:%.obj=%.d)" -MT="$(@:%.obj=%.obj)" -MT="$(@:%.obj=%.d)" -lang=c99   -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01\r_lvd_rx","C:\Workspace\e2studio\git\MT01\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01/r_usb_basic","C:\Workspace\e2studio\git\MT01\fsystem","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc","C:/Workspace/e2studio/git/MT01/r_bsp","C:/Workspace/e2studio/git/MT01/r_dtc_rx","C:/Workspace/e2studio/git/MT01/r_config","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01\r_cmt_rx","C:\Workspace\e2studio\git\MT01\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01/src/include","C:\Workspace\e2studio\git\MT01\src\cnc","C:\Workspace\e2studio\git\MT01\r_config","C:\Workspace\e2studio\git\MT01\r_byteq","C:\Workspace\e2studio\git\MT01\r_byteq\src","C:\Workspace\e2studio\git\MT01\r_rspi_rx","C:\Workspace\e2studio\git\MT01\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01\r_mtu_rx","C:\Workspace\e2studio\git\MT01\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01\r_tmr_rx","C:\Workspace\e2studio\git\MT01\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01\r_flash_api_rx","C:\Workspace\e2studio\git\MT01\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01\r_vee","C:\Workspace\e2studio\git\MT01\r_vee\src","C:\Workspace\e2studio\git\MT01\src\cnc\macros","C:\Workspace\e2studio\git\MT01\r_s12ad_rx","C:\Workspace\e2studio\git\MT01\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01\r_spi_flash","C:\Workspace\e2studio\git\MT01\r_spi_flash\src","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01\r_crc_rx","C:\Workspace\e2studio\git\MT01\r_crc_rx\src","C:\Workspace\e2studio\git\MT01\fsystem_spi","C:\Workspace\e2studio\git\MT01\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=0,EASYMAK=1,MOBILE=0,FREE_RTOS_PP   "$<"
	ccrx -lang=c99 -output=obj="$(@:%.d=%.obj)"  -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01\r_lvd_rx","C:\Workspace\e2studio\git\MT01\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01/r_usb_basic","C:\Workspace\e2studio\git\MT01\fsystem","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc","C:/Workspace/e2studio/git/MT01/r_bsp","C:/Workspace/e2studio/git/MT01/r_dtc_rx","C:/Workspace/e2studio/git/MT01/r_config","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01\r_cmt_rx","C:\Workspace\e2studio\git\MT01\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01/src/include","C:\Workspace\e2studio\git\MT01\src\cnc","C:\Workspace\e2studio\git\MT01\r_config","C:\Workspace\e2studio\git\MT01\r_byteq","C:\Workspace\e2studio\git\MT01\r_byteq\src","C:\Workspace\e2studio\git\MT01\r_rspi_rx","C:\Workspace\e2studio\git\MT01\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01\r_mtu_rx","C:\Workspace\e2studio\git\MT01\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01\r_tmr_rx","C:\Workspace\e2studio\git\MT01\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01\r_flash_api_rx","C:\Workspace\e2studio\git\MT01\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01\r_vee","C:\Workspace\e2studio\git\MT01\r_vee\src","C:\Workspace\e2studio\git\MT01\src\cnc\macros","C:\Workspace\e2studio\git\MT01\r_s12ad_rx","C:\Workspace\e2studio\git\MT01\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01\r_spi_flash","C:\Workspace\e2studio\git\MT01\r_spi_flash\src","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01\r_crc_rx","C:\Workspace\e2studio\git\MT01\r_crc_rx\src","C:\Workspace\e2studio\git\MT01\fsystem_spi","C:\Workspace\e2studio\git\MT01\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=0,EASYMAK=1,MOBILE=0,FREE_RTOS_PP   "$<"
	@echo 'Finished scanning and building: $<'
	@echo.

