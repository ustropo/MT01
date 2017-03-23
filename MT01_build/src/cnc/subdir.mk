################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
..\src/cnc/canonical_machine.c \
..\src/cnc/config.c \
..\src/cnc/config_app.c \
..\src/cnc/controller.c \
..\src/cnc/cycle_homing.c \
..\src/cnc/cycle_jogging.c \
..\src/cnc/cycle_probing.c \
..\src/cnc/cycle_waiting_switch.c \
..\src/cnc/encoder.c \
..\src/cnc/gcode_parser.c \
..\src/cnc/gpio.c \
..\src/cnc/hardware.c \
..\src/cnc/help.c \
..\src/cnc/json_parser.c \
..\src/cnc/kinematics.c \
..\src/cnc/maincnc_task.c \
..\src/cnc/network.c \
..\src/cnc/persistence.c \
..\src/cnc/plan_arc.c \
..\src/cnc/plan_exec.c \
..\src/cnc/plan_line.c \
..\src/cnc/plan_zoid.c \
..\src/cnc/planner.c \
..\src/cnc/plasma.c \
..\src/cnc/pwm.c \
..\src/cnc/report.c \
..\src/cnc/spindle.c \
..\src/cnc/stepper.c \
..\src/cnc/switch.c \
..\src/cnc/system.c \
..\src/cnc/test.c \
..\src/cnc/text_parser.c \
..\src/cnc/util.c \
..\src/cnc/xio.c 

C_DEPS += \
./src/cnc/canonical_machine.d \
./src/cnc/config.d \
./src/cnc/config_app.d \
./src/cnc/controller.d \
./src/cnc/cycle_homing.d \
./src/cnc/cycle_jogging.d \
./src/cnc/cycle_probing.d \
./src/cnc/cycle_waiting_switch.d \
./src/cnc/encoder.d \
./src/cnc/gcode_parser.d \
./src/cnc/gpio.d \
./src/cnc/hardware.d \
./src/cnc/help.d \
./src/cnc/json_parser.d \
./src/cnc/kinematics.d \
./src/cnc/maincnc_task.d \
./src/cnc/network.d \
./src/cnc/persistence.d \
./src/cnc/plan_arc.d \
./src/cnc/plan_exec.d \
./src/cnc/plan_line.d \
./src/cnc/plan_zoid.d \
./src/cnc/planner.d \
./src/cnc/plasma.d \
./src/cnc/pwm.d \
./src/cnc/report.d \
./src/cnc/spindle.d \
./src/cnc/stepper.d \
./src/cnc/switch.d \
./src/cnc/system.d \
./src/cnc/test.d \
./src/cnc/text_parser.d \
./src/cnc/util.d \
./src/cnc/xio.d 

OBJS += \
./src/cnc/canonical_machine.obj \
./src/cnc/config.obj \
./src/cnc/config_app.obj \
./src/cnc/controller.obj \
./src/cnc/cycle_homing.obj \
./src/cnc/cycle_jogging.obj \
./src/cnc/cycle_probing.obj \
./src/cnc/cycle_waiting_switch.obj \
./src/cnc/encoder.obj \
./src/cnc/gcode_parser.obj \
./src/cnc/gpio.obj \
./src/cnc/hardware.obj \
./src/cnc/help.obj \
./src/cnc/json_parser.obj \
./src/cnc/kinematics.obj \
./src/cnc/maincnc_task.obj \
./src/cnc/network.obj \
./src/cnc/persistence.obj \
./src/cnc/plan_arc.obj \
./src/cnc/plan_exec.obj \
./src/cnc/plan_line.obj \
./src/cnc/plan_zoid.obj \
./src/cnc/planner.obj \
./src/cnc/plasma.obj \
./src/cnc/pwm.obj \
./src/cnc/report.obj \
./src/cnc/spindle.obj \
./src/cnc/stepper.obj \
./src/cnc/switch.obj \
./src/cnc/system.obj \
./src/cnc/test.obj \
./src/cnc/text_parser.obj \
./src/cnc/util.obj \
./src/cnc/xio.obj 


# Each subdirectory must supply rules for building sources it contributes
src/cnc/%.obj: ../src/cnc/%.c src/cnc/c.sub
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	ccrx  -MM -MP -output=dep="$(@:%.obj=%.d)" -MT="$(@:%.obj=%.obj)" -MT="$(@:%.obj=%.d)" -lang=c99   -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01_master/r_usb_basic","C:\Workspace\e2studio\git\MT01_master\fsystem","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc","C:/Workspace/e2studio/git/MT01_master/r_bsp","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx","C:/Workspace/e2studio/git/MT01_master/r_config","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01_master/src/include","C:\Workspace\e2studio\git\MT01_master\src\cnc","C:\Workspace\e2studio\git\MT01_master\r_config","C:\Workspace\e2studio\git\MT01_master\r_byteq","C:\Workspace\e2studio\git\MT01_master\r_byteq\src","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01_master\r_vee","C:\Workspace\e2studio\git\MT01_master\r_vee\src","C:\Workspace\e2studio\git\MT01_master\src\cnc\macros","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01_master\r_spi_flash","C:\Workspace\e2studio\git\MT01_master\r_spi_flash\src","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01_master\r_crc_rx","C:\Workspace\e2studio\git\MT01_master\r_crc_rx\src","C:\Workspace\e2studio\git\MT01_master\fsystem_spi","C:\Workspace\e2studio\git\MT01_master\spiffs","C:\Workspace\e2studio\git\MT01_master\r_sci_async_rx","C:\Workspace\e2studio\git\MT01_master\r_sci_async_rx\src","C:\Workspace\e2studio\git\MT01_master\src\Nextion"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=0,EASYMAK=1,MOBILE=0,FREE_RTOS_PP   "$<"
	ccrx -lang=c99 -output=obj="$(@:%.d=%.obj)"  -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx","C:\Workspace\e2studio\git\MT01_master\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01_master/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01_master/r_usb_basic","C:\Workspace\e2studio\git\MT01_master\fsystem","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01_master/r_usb_hmsc","C:/Workspace/e2studio/git/MT01_master/r_bsp","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx","C:/Workspace/e2studio/git/MT01_master/r_config","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01_master/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01_master/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx","C:\Workspace\e2studio\git\MT01_master\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01_master/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01_master/src/include","C:\Workspace\e2studio\git\MT01_master\src\cnc","C:\Workspace\e2studio\git\MT01_master\r_config","C:\Workspace\e2studio\git\MT01_master\r_byteq","C:\Workspace\e2studio\git\MT01_master\r_byteq\src","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx","C:\Workspace\e2studio\git\MT01_master\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx","C:\Workspace\e2studio\git\MT01_master\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx","C:\Workspace\e2studio\git\MT01_master\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01_master\r_vee","C:\Workspace\e2studio\git\MT01_master\r_vee\src","C:\Workspace\e2studio\git\MT01_master\src\cnc\macros","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx","C:\Workspace\e2studio\git\MT01_master\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01_master\r_spi_flash","C:\Workspace\e2studio\git\MT01_master\r_spi_flash\src","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01_master\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01_master\r_crc_rx","C:\Workspace\e2studio\git\MT01_master\r_crc_rx\src","C:\Workspace\e2studio\git\MT01_master\fsystem_spi","C:\Workspace\e2studio\git\MT01_master\spiffs","C:\Workspace\e2studio\git\MT01_master\r_sci_async_rx","C:\Workspace\e2studio\git\MT01_master\r_sci_async_rx\src","C:\Workspace\e2studio\git\MT01_master\src\Nextion"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=0,EASYMAK=1,MOBILE=0,FREE_RTOS_PP   "$<"
	@echo 'Finished scanning and building: $<'
	@echo.

