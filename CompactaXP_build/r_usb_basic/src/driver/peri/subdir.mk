################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
..\r_usb_basic/src/driver/peri/r_usb_pbc.c \
..\r_usb_basic/src/driver/peri/r_usb_pcontrolrw.c \
..\r_usb_basic/src/driver/peri/r_usb_pdriver.c \
..\r_usb_basic/src/driver/peri/r_usb_pdriverapi.c \
..\r_usb_basic/src/driver/peri/r_usb_pintfifo.c \
..\r_usb_basic/src/driver/peri/r_usb_psignal.c \
..\r_usb_basic/src/driver/peri/r_usb_pstdfunction.c \
..\r_usb_basic/src/driver/peri/r_usb_pstdrequest.c 

C_DEPS += \
./r_usb_basic/src/driver/peri/r_usb_pbc.d \
./r_usb_basic/src/driver/peri/r_usb_pcontrolrw.d \
./r_usb_basic/src/driver/peri/r_usb_pdriver.d \
./r_usb_basic/src/driver/peri/r_usb_pdriverapi.d \
./r_usb_basic/src/driver/peri/r_usb_pintfifo.d \
./r_usb_basic/src/driver/peri/r_usb_psignal.d \
./r_usb_basic/src/driver/peri/r_usb_pstdfunction.d \
./r_usb_basic/src/driver/peri/r_usb_pstdrequest.d 

OBJS += \
./r_usb_basic/src/driver/peri/r_usb_pbc.obj \
./r_usb_basic/src/driver/peri/r_usb_pcontrolrw.obj \
./r_usb_basic/src/driver/peri/r_usb_pdriver.obj \
./r_usb_basic/src/driver/peri/r_usb_pdriverapi.obj \
./r_usb_basic/src/driver/peri/r_usb_pintfifo.obj \
./r_usb_basic/src/driver/peri/r_usb_psignal.obj \
./r_usb_basic/src/driver/peri/r_usb_pstdfunction.obj \
./r_usb_basic/src/driver/peri/r_usb_pstdrequest.obj 


# Each subdirectory must supply rules for building sources it contributes
r_usb_basic/src/driver/peri/%.obj: ../r_usb_basic/src/driver/peri/%.c r_usb_basic/src/driver/peri/c.sub
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	ccrx  -MM -MP -output=dep="$(@:%.obj=%.d)" -MT="$(@:%.obj=%.obj)" -MT="$(@:%.obj=%.d)" -lang=c99   -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01_master/r_usb_basic","C:\Workspace\e2studio\git\MT01_master\fsystem","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc","C:/Workspace/e2studio/git/MT01_master/r_bsp","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx","C:/Workspace/e2studio/git/MT01_master/r_config","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01_master/src/include","C:\Workspace\e2studio\git\MT01_master\src\cnc","C:\Workspace\e2studio\git\MT01_master\r_config","C:\Workspace\e2studio\git\MT01_master\r_byteq","C:\Workspace\e2studio\git\MT01_master\r_byteq\src","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01_master\r_vee","C:\Workspace\e2studio\git\MT01_master\r_vee\src","C:\Workspace\e2studio\git\MT01_master\src\cnc\macros","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01_master\r_spi_flash","C:\Workspace\e2studio\git\MT01_master\r_spi_flash\src","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01_master\r_crc_rx","C:\Workspace\e2studio\git\MT01_master\r_crc_rx\src","C:\Workspace\e2studio\git\MT01_master\fsystem_spi","C:\Workspace\e2studio\git\MT01_master\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=1,EASYMAK=0,MOBILE=0,FREE_RTOS_PP   "$<"
	ccrx -lang=c99 -output=obj="$(@:%.d=%.obj)"  -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01_master/r_usb_basic","C:\Workspace\e2studio\git\MT01_master\fsystem","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc","C:/Workspace/e2studio/git/MT01_master/r_bsp","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx","C:/Workspace/e2studio/git/MT01_master/r_config","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01_master/src/include","C:\Workspace\e2studio\git\MT01_master\src\cnc","C:\Workspace\e2studio\git\MT01_master\r_config","C:\Workspace\e2studio\git\MT01_master\r_byteq","C:\Workspace\e2studio\git\MT01_master\r_byteq\src","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01_master\r_vee","C:\Workspace\e2studio\git\MT01_master\r_vee\src","C:\Workspace\e2studio\git\MT01_master\src\cnc\macros","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01_master\r_spi_flash","C:\Workspace\e2studio\git\MT01_master\r_spi_flash\src","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01_master\r_crc_rx","C:\Workspace\e2studio\git\MT01_master\r_crc_rx\src","C:\Workspace\e2studio\git\MT01_master\fsystem_spi","C:\Workspace\e2studio\git\MT01_master\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=1,EASYMAK=0,MOBILE=0,FREE_RTOS_PP   "$<"
	@echo 'Finished scanning and building: $<'
	@echo.

