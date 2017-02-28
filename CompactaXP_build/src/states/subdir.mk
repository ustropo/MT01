################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
..\src/states/config_maquina.c \
..\src/states/config_menu_ox.c \
..\src/states/config_menu_pl.c \
..\src/states/config_par_maquina.c \
..\src/states/config_thc_maquina.c \
..\src/states/state_functions.c \
..\src/states/ut_state_choose_file.c \
..\src/states/ut_state_config_auto.c \
..\src/states/ut_state_config_jog.c \
..\src/states/ut_state_config_manual.c \
..\src/states/ut_state_config_maq_thc.c \
..\src/states/ut_state_config_maquina.c \
..\src/states/ut_state_config_menu.c \
..\src/states/ut_state_config_par_maq.c \
..\src/states/ut_state_config_var.c \
..\src/states/ut_state_line_selection.c \
..\src/states/ut_state_main_menu.c \
..\src/states/ut_state_move.c \
..\src/states/ut_state_splash.c \
..\src/states/ut_state_warning.c \
..\src/states/ut_states_map.c \
..\src/states/ut_states_task.c 

C_DEPS += \
./src/states/config_maquina.d \
./src/states/config_menu_ox.d \
./src/states/config_menu_pl.d \
./src/states/config_par_maquina.d \
./src/states/config_thc_maquina.d \
./src/states/state_functions.d \
./src/states/ut_state_choose_file.d \
./src/states/ut_state_config_auto.d \
./src/states/ut_state_config_jog.d \
./src/states/ut_state_config_manual.d \
./src/states/ut_state_config_maq_thc.d \
./src/states/ut_state_config_maquina.d \
./src/states/ut_state_config_menu.d \
./src/states/ut_state_config_par_maq.d \
./src/states/ut_state_config_var.d \
./src/states/ut_state_line_selection.d \
./src/states/ut_state_main_menu.d \
./src/states/ut_state_move.d \
./src/states/ut_state_splash.d \
./src/states/ut_state_warning.d \
./src/states/ut_states_map.d \
./src/states/ut_states_task.d 

OBJS += \
./src/states/config_maquina.obj \
./src/states/config_menu_ox.obj \
./src/states/config_menu_pl.obj \
./src/states/config_par_maquina.obj \
./src/states/config_thc_maquina.obj \
./src/states/state_functions.obj \
./src/states/ut_state_choose_file.obj \
./src/states/ut_state_config_auto.obj \
./src/states/ut_state_config_jog.obj \
./src/states/ut_state_config_manual.obj \
./src/states/ut_state_config_maq_thc.obj \
./src/states/ut_state_config_maquina.obj \
./src/states/ut_state_config_menu.obj \
./src/states/ut_state_config_par_maq.obj \
./src/states/ut_state_config_var.obj \
./src/states/ut_state_line_selection.obj \
./src/states/ut_state_main_menu.obj \
./src/states/ut_state_move.obj \
./src/states/ut_state_splash.obj \
./src/states/ut_state_warning.obj \
./src/states/ut_states_map.obj \
./src/states/ut_states_task.obj 


# Each subdirectory must supply rules for building sources it contributes
src/states/%.obj: ../src/states/%.c src/states/c.sub
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	ccrx  -MM -MP -output=dep="$(@:%.obj=%.d)" -MT="$(@:%.obj=%.obj)" -MT="$(@:%.obj=%.d)" -lang=c99   -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01\r_lvd_rx","C:\Workspace\e2studio\git\MT01\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01/r_usb_basic","C:\Workspace\e2studio\git\MT01\fsystem","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc","C:/Workspace/e2studio/git/MT01/r_bsp","C:/Workspace/e2studio/git/MT01/r_dtc_rx","C:/Workspace/e2studio/git/MT01/r_config","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01\r_cmt_rx","C:\Workspace\e2studio\git\MT01\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01/src/include","C:\Workspace\e2studio\git\MT01\src\cnc","C:\Workspace\e2studio\git\MT01\r_config","C:\Workspace\e2studio\git\MT01\r_byteq","C:\Workspace\e2studio\git\MT01\r_byteq\src","C:\Workspace\e2studio\git\MT01\r_rspi_rx","C:\Workspace\e2studio\git\MT01\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01\r_mtu_rx","C:\Workspace\e2studio\git\MT01\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01\r_tmr_rx","C:\Workspace\e2studio\git\MT01\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01\r_flash_api_rx","C:\Workspace\e2studio\git\MT01\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01\r_vee","C:\Workspace\e2studio\git\MT01\r_vee\src","C:\Workspace\e2studio\git\MT01\src\cnc\macros","C:\Workspace\e2studio\git\MT01\r_s12ad_rx","C:\Workspace\e2studio\git\MT01\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01\r_spi_flash","C:\Workspace\e2studio\git\MT01\r_spi_flash\src","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01\r_crc_rx","C:\Workspace\e2studio\git\MT01\r_crc_rx\src","C:\Workspace\e2studio\git\MT01\fsystem_spi","C:\Workspace\e2studio\git\MT01\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=1,EASYMAK=0,MOBILE=0,FREE_RTOS_PP   "$<"
	ccrx -lang=c99 -output=obj="$(@:%.d=%.obj)"  -include="C:\PROGRA~2\Renesas\RX\2_3_0/include","C:\Workspace\e2studio\git\MT01\r_lvd_rx","C:\Workspace\e2studio\git\MT01\r_lvd_rx\src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx/src","C:/Workspace/e2studio/git/MT01/r_tfat_driver_rx","C:/Workspace/e2studio/git/MT01/r_usb_basic","C:\Workspace\e2studio\git\MT01\fsystem","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/HW/inc","C:/Workspace/e2studio/git/MT01/r_usb_basic/src/driver/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc/src/inc","C:/Workspace/e2studio/git/MT01/r_usb_hmsc","C:/Workspace/e2studio/git/MT01/r_bsp","C:/Workspace/e2studio/git/MT01/r_dtc_rx","C:/Workspace/e2studio/git/MT01/r_config","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src","C:/Workspace/e2studio/git/MT01/r_dtc_rx/src/targets/rx63n","C:/Workspace/e2studio/git/MT01/r_bsp/board/rdkrx63n","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n/register_access","C:/Workspace/e2studio/git/MT01/r_bsp/mcu/rx63n","C:\Workspace\e2studio\git\MT01\r_cmt_rx","C:\Workspace\e2studio\git\MT01\r_cmt_rx\src","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/include","C:/Workspace/e2studio/git/MT01/FreeRTOS/Source/portable/Renesas/RX600","C:/Workspace/e2studio/git/MT01/src/include","C:\Workspace\e2studio\git\MT01\src\cnc","C:\Workspace\e2studio\git\MT01\r_config","C:\Workspace\e2studio\git\MT01\r_byteq","C:\Workspace\e2studio\git\MT01\r_byteq\src","C:\Workspace\e2studio\git\MT01\r_rspi_rx","C:\Workspace\e2studio\git\MT01\r_rspi_rx\src","C:\Workspace\e2studio\git\MT01\r_mtu_rx","C:\Workspace\e2studio\git\MT01\r_mtu_rx\src","C:\Workspace\e2studio\git\MT01\r_tmr_rx","C:\Workspace\e2studio\git\MT01\r_tmr_rx\src","C:\Workspace\e2studio\git\MT01\r_flash_api_rx","C:\Workspace\e2studio\git\MT01\r_flash_api_rx\src","C:\Workspace\e2studio\git\MT01\r_vee","C:\Workspace\e2studio\git\MT01\r_vee\src","C:\Workspace\e2studio\git\MT01\src\cnc\macros","C:\Workspace\e2studio\git\MT01\r_s12ad_rx","C:\Workspace\e2studio\git\MT01\r_s12ad_rx\src","C:\Workspace\e2studio\git\MT01\r_spi_flash","C:\Workspace\e2studio\git\MT01\r_spi_flash\src","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx","C:\Workspace\e2studio\git\MT01\r_flash_loader_rx\src","C:\Workspace\e2studio\git\MT01\r_crc_rx","C:\Workspace\e2studio\git\MT01\r_crc_rx\src","C:\Workspace\e2studio\git\MT01\fsystem_spi","C:\Workspace\e2studio\git\MT01\spiffs"  -debug -show=source,conditionals,definitions,expansions -listfile="$(basename $(notdir $<)).lst" -isa=rxv1 -optimize=0 -fpu -alias=noansi -nologo -nomessage  -define=__RX,COMPACTAXP=1,EASYMAK=0,MOBILE=0,FREE_RTOS_PP   "$<"
	@echo 'Finished scanning and building: $<'
	@echo.

