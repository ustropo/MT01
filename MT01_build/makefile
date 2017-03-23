################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf *.sni *.dbg *.lst *.lis *.lpp *.map *.x *.lbp src/*.src src/*.p src/*.pp *.bls *.libelf *.lbk *.abs *.fsy

# All of the sources participating in the build are defined here
-include sources.mk
-include src/states/subdir.mk
-include src/subdir.mk
-include src/keyboard/subdir.mk
-include src/eeprom/subdir.mk
-include src/config/subdir.mk
-include src/cnc/subdir.mk
-include src/cnc/xio/subdir.mk
-include src/cnc/macros/subdir.mk
-include src/Nextion/subdir.mk
-include src/Display/subdir.mk
-include src/Display/font/subdir.mk
-include spiffs/subdir.mk
-include r_vee/src/targets/rx63x/subdir.mk
-include r_vee/src/subdir.mk
-include r_usb_hmsc/src/subdir.mk
-include r_usb_basic/src/driver/peri/subdir.mk
-include r_usb_basic/src/driver/host/subdir.mk
-include r_usb_basic/src/driver/comm/subdir.mk
-include r_usb_basic/src/HW/peri/subdir.mk
-include r_usb_basic/src/HW/host/subdir.mk
-include r_usb_basic/src/HW/comm/subdir.mk
-include r_tmr_rx/src/subdir.mk
-include r_tfat_driver_rx/src/subdir.mk
-include r_tfat_driver_rx/src/device/usb_mini/subdir.mk
-include r_tfat_driver_rx/src/device/usb/subdir.mk
-include r_tfat_driver_rx/src/device/sdhi/subdir.mk
-include r_spi_flash/src/subdir.mk
-include r_sci_async_rx/src/subdir.mk
-include r_s12ad_rx/src/targets/rx63x/subdir.mk
-include r_s12ad_rx/src/targets/rx210/subdir.mk
-include r_s12ad_rx/src/targets/rx110_rx111/subdir.mk
-include r_s12ad_rx/src/subdir.mk
-include r_rspi_rx/src/subdir.mk
-include r_mtu_rx/src/subdir.mk
-include r_lvd_rx/src/subdir.mk
-include r_flash_loader_rx/src/subdir.mk
-include r_flash_loader_rx/src/memory/subdir.mk
-include r_flash_api_rx/src/subdir.mk
-include r_dtc_rx/src/targets/rx71m/subdir.mk
-include r_dtc_rx/src/targets/rx64m/subdir.mk
-include r_dtc_rx/src/targets/rx63n/subdir.mk
-include r_dtc_rx/src/targets/rx113/subdir.mk
-include r_dtc_rx/src/targets/rx111/subdir.mk
-include r_dtc_rx/src/targets/rx110/subdir.mk
-include r_dtc_rx/src/subdir.mk
-include r_crc_rx/src/subdir.mk
-include r_cmt_rx/src/subdir.mk
-include r_byteq/src/subdir.mk
-include r_bsp/mcu/rx63n/subdir.mk
-include r_bsp/mcu/all/subdir.mk
-include r_bsp/board/MT01/subdir.mk
-include fsystem_spi/subdir.mk
-include fsystem/subdir.mk
-include FreeRTOS/Source/subdir.mk
-include FreeRTOS/Source/portable/Renesas/RX600/subdir.mk
-include FreeRTOS/Source/portable/MemMang/subdir.mk
-include .metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_UPPER_DEPS)),)
-include $(CC_UPPER_DEPS)
endif
ifneq ($(strip $(C++_UPPER_DEPS)),)
-include $(C++_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_UPPER_DEPS)),)
-include $(CXX_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_UPPER_DEPS)),)
-include $(CPP_UPPER_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CP_DEPS)),)
-include $(CP_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(CP_UPPER_DEPS)),)
-include $(CP_UPPER_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(OBJ_DEPS)),)
-include $(OBJ_DEPS)
endif
ifneq ($(strip $(ABS_DEPS)),)
-include $(ABS_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(O_DEPS)),)
-include $(O_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

ifneq ($(strip $(SRC_UPPER_SRCS)),)
ALL_ASMS += $(SRC_UPPER_SRCS) 
endif
ifneq ($(strip $(SRC_SRCS)),)
ALL_ASMS += $(SRC_SRCS) 
endif
ifneq ($(strip $(S_UPPER_SRCS)),)
ALL_ASMS += $(S_UPPER_SRCS) 
endif
ifneq ($(strip $(S_SRCS)),)
ALL_ASMS += $(S_SRCS) 
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
CONVERTOR_OUTPUT_OUTPUTS += \
MT01_proj.mot \

LINKER_OUTPUT_OUTPUTS += \
MT01_proj.abs \

LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS += \
MT01_proj.lib \

# All Target
all: MT01_proj.mot
	@echo 'Build complete.'

# Tool invocations
MT01_proj.mot: $(LINKER_OUTPUT_OUTPUTS)
	@echo 'Invoking: Converter'
	rlink   "C:\Workspace\e2studio\git\MT01_master\MT01_build\MT01_proj.abs" -form=stype  -output="C:\Workspace\e2studio\git\MT01_master\MT01_build\MT01_proj.mot" -crc=FFFFFE05=FFF00000-FFFFFE04,FFFFFE07-FFFFFFFF:little
	@echo 'Finished building target:'
	@echo.
	$(MAKE) --no-print-directory post-build

MT01_proj.abs: $(OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(ALL_ASMS) 
	@echo 'Invoking: Linker'
	@echo 'Building target:'
	rlink  -subcommand="LinkerSubCommand.tmp"  -list="MT01_proj.map" -show=symbol,reference,total_size -nooptimize -debug -output="C:\Workspace\e2studio\git\MT01_master\MT01_build\MT01_proj.abs" -rom=D=R,D_1=R_1,D_2=R_2 -nocompress -nologo -nomessage
	@echo 'Finished building:'
	@echo.

MT01_proj.lib:  
	@echo 'Invoking: Standard Library'
	lbgrx -lang=c99 -head=runtime,ctype,math,stdio,stdlib,string,new,cppstring -isa=rxv1 -fpu -nologo -output="MT01_proj.lib"
	@echo 'Finished building:'
	@echo.

# Other Targets
clean:
	-$(RM) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(CONVERTOR_OUTPUT_OUTPUTS) 
	-rm -rf "src/states"/*.obj "src/states"/*.d "src/states"/*.src "src/states"/*.p "src/states"/*.pp "src/states"/*.mrc "src/states"/*.prn
	-rm -rf "src"/*.obj "src"/*.d "src"/*.src "src"/*.p "src"/*.pp "src"/*.mrc "src"/*.prn
	-rm -rf "src/keyboard"/*.obj "src/keyboard"/*.d "src/keyboard"/*.src "src/keyboard"/*.p "src/keyboard"/*.pp "src/keyboard"/*.mrc "src/keyboard"/*.prn
	-rm -rf "src/eeprom"/*.obj "src/eeprom"/*.d "src/eeprom"/*.src "src/eeprom"/*.p "src/eeprom"/*.pp "src/eeprom"/*.mrc "src/eeprom"/*.prn
	-rm -rf "src/config"/*.obj "src/config"/*.d "src/config"/*.src "src/config"/*.p "src/config"/*.pp "src/config"/*.mrc "src/config"/*.prn
	-rm -rf "src/cnc"/*.obj "src/cnc"/*.d "src/cnc"/*.src "src/cnc"/*.p "src/cnc"/*.pp "src/cnc"/*.mrc "src/cnc"/*.prn
	-rm -rf "src/cnc/xio"/*.obj "src/cnc/xio"/*.d "src/cnc/xio"/*.src "src/cnc/xio"/*.p "src/cnc/xio"/*.pp "src/cnc/xio"/*.mrc "src/cnc/xio"/*.prn
	-rm -rf "src/cnc/macros"/*.obj "src/cnc/macros"/*.d "src/cnc/macros"/*.src "src/cnc/macros"/*.p "src/cnc/macros"/*.pp "src/cnc/macros"/*.mrc "src/cnc/macros"/*.prn
	-rm -rf "src/Nextion"/*.obj "src/Nextion"/*.d "src/Nextion"/*.src "src/Nextion"/*.p "src/Nextion"/*.pp "src/Nextion"/*.mrc "src/Nextion"/*.prn
	-rm -rf "src/Display"/*.obj "src/Display"/*.d "src/Display"/*.src "src/Display"/*.p "src/Display"/*.pp "src/Display"/*.mrc "src/Display"/*.prn
	-rm -rf "src/Display/font"/*.obj "src/Display/font"/*.d "src/Display/font"/*.src "src/Display/font"/*.p "src/Display/font"/*.pp "src/Display/font"/*.mrc "src/Display/font"/*.prn
	-rm -rf "spiffs"/*.obj "spiffs"/*.d "spiffs"/*.src "spiffs"/*.p "spiffs"/*.pp "spiffs"/*.mrc "spiffs"/*.prn
	-rm -rf "r_vee/src/targets/rx63x"/*.obj "r_vee/src/targets/rx63x"/*.d "r_vee/src/targets/rx63x"/*.src "r_vee/src/targets/rx63x"/*.p "r_vee/src/targets/rx63x"/*.pp "r_vee/src/targets/rx63x"/*.mrc "r_vee/src/targets/rx63x"/*.prn
	-rm -rf "r_vee/src"/*.obj "r_vee/src"/*.d "r_vee/src"/*.src "r_vee/src"/*.p "r_vee/src"/*.pp "r_vee/src"/*.mrc "r_vee/src"/*.prn
	-rm -rf "r_usb_hmsc/src"/*.obj "r_usb_hmsc/src"/*.d "r_usb_hmsc/src"/*.src "r_usb_hmsc/src"/*.p "r_usb_hmsc/src"/*.pp "r_usb_hmsc/src"/*.mrc "r_usb_hmsc/src"/*.prn
	-rm -rf "r_usb_basic/src/driver/peri"/*.obj "r_usb_basic/src/driver/peri"/*.d "r_usb_basic/src/driver/peri"/*.src "r_usb_basic/src/driver/peri"/*.p "r_usb_basic/src/driver/peri"/*.pp "r_usb_basic/src/driver/peri"/*.mrc "r_usb_basic/src/driver/peri"/*.prn
	-rm -rf "r_usb_basic/src/driver/host"/*.obj "r_usb_basic/src/driver/host"/*.d "r_usb_basic/src/driver/host"/*.src "r_usb_basic/src/driver/host"/*.p "r_usb_basic/src/driver/host"/*.pp "r_usb_basic/src/driver/host"/*.mrc "r_usb_basic/src/driver/host"/*.prn
	-rm -rf "r_usb_basic/src/driver/comm"/*.obj "r_usb_basic/src/driver/comm"/*.d "r_usb_basic/src/driver/comm"/*.src "r_usb_basic/src/driver/comm"/*.p "r_usb_basic/src/driver/comm"/*.pp "r_usb_basic/src/driver/comm"/*.mrc "r_usb_basic/src/driver/comm"/*.prn
	-rm -rf "r_usb_basic/src/HW/peri"/*.obj "r_usb_basic/src/HW/peri"/*.d "r_usb_basic/src/HW/peri"/*.src "r_usb_basic/src/HW/peri"/*.p "r_usb_basic/src/HW/peri"/*.pp "r_usb_basic/src/HW/peri"/*.mrc "r_usb_basic/src/HW/peri"/*.prn
	-rm -rf "r_usb_basic/src/HW/host"/*.obj "r_usb_basic/src/HW/host"/*.d "r_usb_basic/src/HW/host"/*.src "r_usb_basic/src/HW/host"/*.p "r_usb_basic/src/HW/host"/*.pp "r_usb_basic/src/HW/host"/*.mrc "r_usb_basic/src/HW/host"/*.prn
	-rm -rf "r_usb_basic/src/HW/comm"/*.obj "r_usb_basic/src/HW/comm"/*.d "r_usb_basic/src/HW/comm"/*.src "r_usb_basic/src/HW/comm"/*.p "r_usb_basic/src/HW/comm"/*.pp "r_usb_basic/src/HW/comm"/*.mrc "r_usb_basic/src/HW/comm"/*.prn
	-rm -rf "r_tmr_rx/src"/*.obj "r_tmr_rx/src"/*.d "r_tmr_rx/src"/*.src "r_tmr_rx/src"/*.p "r_tmr_rx/src"/*.pp "r_tmr_rx/src"/*.mrc "r_tmr_rx/src"/*.prn
	-rm -rf "r_tfat_driver_rx/src"/*.obj "r_tfat_driver_rx/src"/*.d "r_tfat_driver_rx/src"/*.src "r_tfat_driver_rx/src"/*.p "r_tfat_driver_rx/src"/*.pp "r_tfat_driver_rx/src"/*.mrc "r_tfat_driver_rx/src"/*.prn
	-rm -rf "r_tfat_driver_rx/src/device/usb_mini"/*.obj "r_tfat_driver_rx/src/device/usb_mini"/*.d "r_tfat_driver_rx/src/device/usb_mini"/*.src "r_tfat_driver_rx/src/device/usb_mini"/*.p "r_tfat_driver_rx/src/device/usb_mini"/*.pp "r_tfat_driver_rx/src/device/usb_mini"/*.mrc "r_tfat_driver_rx/src/device/usb_mini"/*.prn
	-rm -rf "r_tfat_driver_rx/src/device/usb"/*.obj "r_tfat_driver_rx/src/device/usb"/*.d "r_tfat_driver_rx/src/device/usb"/*.src "r_tfat_driver_rx/src/device/usb"/*.p "r_tfat_driver_rx/src/device/usb"/*.pp "r_tfat_driver_rx/src/device/usb"/*.mrc "r_tfat_driver_rx/src/device/usb"/*.prn
	-rm -rf "r_tfat_driver_rx/src/device/sdhi"/*.obj "r_tfat_driver_rx/src/device/sdhi"/*.d "r_tfat_driver_rx/src/device/sdhi"/*.src "r_tfat_driver_rx/src/device/sdhi"/*.p "r_tfat_driver_rx/src/device/sdhi"/*.pp "r_tfat_driver_rx/src/device/sdhi"/*.mrc "r_tfat_driver_rx/src/device/sdhi"/*.prn
	-rm -rf "r_spi_flash/src"/*.obj "r_spi_flash/src"/*.d "r_spi_flash/src"/*.src "r_spi_flash/src"/*.p "r_spi_flash/src"/*.pp "r_spi_flash/src"/*.mrc "r_spi_flash/src"/*.prn
	-rm -rf "r_sci_async_rx/src"/*.obj "r_sci_async_rx/src"/*.d "r_sci_async_rx/src"/*.src "r_sci_async_rx/src"/*.p "r_sci_async_rx/src"/*.pp "r_sci_async_rx/src"/*.mrc "r_sci_async_rx/src"/*.prn
	-rm -rf "r_s12ad_rx/src/targets/rx63x"/*.obj "r_s12ad_rx/src/targets/rx63x"/*.d "r_s12ad_rx/src/targets/rx63x"/*.src "r_s12ad_rx/src/targets/rx63x"/*.p "r_s12ad_rx/src/targets/rx63x"/*.pp "r_s12ad_rx/src/targets/rx63x"/*.mrc "r_s12ad_rx/src/targets/rx63x"/*.prn
	-rm -rf "r_s12ad_rx/src/targets/rx210"/*.obj "r_s12ad_rx/src/targets/rx210"/*.d "r_s12ad_rx/src/targets/rx210"/*.src "r_s12ad_rx/src/targets/rx210"/*.p "r_s12ad_rx/src/targets/rx210"/*.pp "r_s12ad_rx/src/targets/rx210"/*.mrc "r_s12ad_rx/src/targets/rx210"/*.prn
	-rm -rf "r_s12ad_rx/src/targets/rx110_rx111"/*.obj "r_s12ad_rx/src/targets/rx110_rx111"/*.d "r_s12ad_rx/src/targets/rx110_rx111"/*.src "r_s12ad_rx/src/targets/rx110_rx111"/*.p "r_s12ad_rx/src/targets/rx110_rx111"/*.pp "r_s12ad_rx/src/targets/rx110_rx111"/*.mrc "r_s12ad_rx/src/targets/rx110_rx111"/*.prn
	-rm -rf "r_s12ad_rx/src"/*.obj "r_s12ad_rx/src"/*.d "r_s12ad_rx/src"/*.src "r_s12ad_rx/src"/*.p "r_s12ad_rx/src"/*.pp "r_s12ad_rx/src"/*.mrc "r_s12ad_rx/src"/*.prn
	-rm -rf "r_rspi_rx/src"/*.obj "r_rspi_rx/src"/*.d "r_rspi_rx/src"/*.src "r_rspi_rx/src"/*.p "r_rspi_rx/src"/*.pp "r_rspi_rx/src"/*.mrc "r_rspi_rx/src"/*.prn
	-rm -rf "r_mtu_rx/src"/*.obj "r_mtu_rx/src"/*.d "r_mtu_rx/src"/*.src "r_mtu_rx/src"/*.p "r_mtu_rx/src"/*.pp "r_mtu_rx/src"/*.mrc "r_mtu_rx/src"/*.prn
	-rm -rf "r_lvd_rx/src"/*.obj "r_lvd_rx/src"/*.d "r_lvd_rx/src"/*.src "r_lvd_rx/src"/*.p "r_lvd_rx/src"/*.pp "r_lvd_rx/src"/*.mrc "r_lvd_rx/src"/*.prn
	-rm -rf "r_flash_loader_rx/src"/*.obj "r_flash_loader_rx/src"/*.d "r_flash_loader_rx/src"/*.src "r_flash_loader_rx/src"/*.p "r_flash_loader_rx/src"/*.pp "r_flash_loader_rx/src"/*.mrc "r_flash_loader_rx/src"/*.prn
	-rm -rf "r_flash_loader_rx/src/memory"/*.obj "r_flash_loader_rx/src/memory"/*.d "r_flash_loader_rx/src/memory"/*.src "r_flash_loader_rx/src/memory"/*.p "r_flash_loader_rx/src/memory"/*.pp "r_flash_loader_rx/src/memory"/*.mrc "r_flash_loader_rx/src/memory"/*.prn
	-rm -rf "r_flash_api_rx/src"/*.obj "r_flash_api_rx/src"/*.d "r_flash_api_rx/src"/*.src "r_flash_api_rx/src"/*.p "r_flash_api_rx/src"/*.pp "r_flash_api_rx/src"/*.mrc "r_flash_api_rx/src"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx71m"/*.obj "r_dtc_rx/src/targets/rx71m"/*.d "r_dtc_rx/src/targets/rx71m"/*.src "r_dtc_rx/src/targets/rx71m"/*.p "r_dtc_rx/src/targets/rx71m"/*.pp "r_dtc_rx/src/targets/rx71m"/*.mrc "r_dtc_rx/src/targets/rx71m"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx64m"/*.obj "r_dtc_rx/src/targets/rx64m"/*.d "r_dtc_rx/src/targets/rx64m"/*.src "r_dtc_rx/src/targets/rx64m"/*.p "r_dtc_rx/src/targets/rx64m"/*.pp "r_dtc_rx/src/targets/rx64m"/*.mrc "r_dtc_rx/src/targets/rx64m"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx63n"/*.obj "r_dtc_rx/src/targets/rx63n"/*.d "r_dtc_rx/src/targets/rx63n"/*.src "r_dtc_rx/src/targets/rx63n"/*.p "r_dtc_rx/src/targets/rx63n"/*.pp "r_dtc_rx/src/targets/rx63n"/*.mrc "r_dtc_rx/src/targets/rx63n"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx113"/*.obj "r_dtc_rx/src/targets/rx113"/*.d "r_dtc_rx/src/targets/rx113"/*.src "r_dtc_rx/src/targets/rx113"/*.p "r_dtc_rx/src/targets/rx113"/*.pp "r_dtc_rx/src/targets/rx113"/*.mrc "r_dtc_rx/src/targets/rx113"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx111"/*.obj "r_dtc_rx/src/targets/rx111"/*.d "r_dtc_rx/src/targets/rx111"/*.src "r_dtc_rx/src/targets/rx111"/*.p "r_dtc_rx/src/targets/rx111"/*.pp "r_dtc_rx/src/targets/rx111"/*.mrc "r_dtc_rx/src/targets/rx111"/*.prn
	-rm -rf "r_dtc_rx/src/targets/rx110"/*.obj "r_dtc_rx/src/targets/rx110"/*.d "r_dtc_rx/src/targets/rx110"/*.src "r_dtc_rx/src/targets/rx110"/*.p "r_dtc_rx/src/targets/rx110"/*.pp "r_dtc_rx/src/targets/rx110"/*.mrc "r_dtc_rx/src/targets/rx110"/*.prn
	-rm -rf "r_dtc_rx/src"/*.obj "r_dtc_rx/src"/*.d "r_dtc_rx/src"/*.src "r_dtc_rx/src"/*.p "r_dtc_rx/src"/*.pp "r_dtc_rx/src"/*.mrc "r_dtc_rx/src"/*.prn
	-rm -rf "r_crc_rx/src"/*.obj "r_crc_rx/src"/*.d "r_crc_rx/src"/*.src "r_crc_rx/src"/*.p "r_crc_rx/src"/*.pp "r_crc_rx/src"/*.mrc "r_crc_rx/src"/*.prn
	-rm -rf "r_cmt_rx/src"/*.obj "r_cmt_rx/src"/*.d "r_cmt_rx/src"/*.src "r_cmt_rx/src"/*.p "r_cmt_rx/src"/*.pp "r_cmt_rx/src"/*.mrc "r_cmt_rx/src"/*.prn
	-rm -rf "r_byteq/src"/*.obj "r_byteq/src"/*.d "r_byteq/src"/*.src "r_byteq/src"/*.p "r_byteq/src"/*.pp "r_byteq/src"/*.mrc "r_byteq/src"/*.prn
	-rm -rf "r_bsp/mcu/rx63n"/*.obj "r_bsp/mcu/rx63n"/*.d "r_bsp/mcu/rx63n"/*.src "r_bsp/mcu/rx63n"/*.p "r_bsp/mcu/rx63n"/*.pp "r_bsp/mcu/rx63n"/*.mrc "r_bsp/mcu/rx63n"/*.prn
	-rm -rf "r_bsp/mcu/all"/*.obj "r_bsp/mcu/all"/*.d "r_bsp/mcu/all"/*.src "r_bsp/mcu/all"/*.p "r_bsp/mcu/all"/*.pp "r_bsp/mcu/all"/*.mrc "r_bsp/mcu/all"/*.prn
	-rm -rf "r_bsp/board/MT01"/*.obj "r_bsp/board/MT01"/*.d "r_bsp/board/MT01"/*.src "r_bsp/board/MT01"/*.p "r_bsp/board/MT01"/*.pp "r_bsp/board/MT01"/*.mrc "r_bsp/board/MT01"/*.prn
	-rm -rf "fsystem_spi"/*.obj "fsystem_spi"/*.d "fsystem_spi"/*.src "fsystem_spi"/*.p "fsystem_spi"/*.pp "fsystem_spi"/*.mrc "fsystem_spi"/*.prn
	-rm -rf "fsystem"/*.obj "fsystem"/*.d "fsystem"/*.src "fsystem"/*.p "fsystem"/*.pp "fsystem"/*.mrc "fsystem"/*.prn
	-rm -rf "FreeRTOS/Source"/*.obj "FreeRTOS/Source"/*.d "FreeRTOS/Source"/*.src "FreeRTOS/Source"/*.p "FreeRTOS/Source"/*.pp "FreeRTOS/Source"/*.mrc "FreeRTOS/Source"/*.prn
	-rm -rf "FreeRTOS/Source/portable/Renesas/RX600"/*.obj "FreeRTOS/Source/portable/Renesas/RX600"/*.d "FreeRTOS/Source/portable/Renesas/RX600"/*.src "FreeRTOS/Source/portable/Renesas/RX600"/*.p "FreeRTOS/Source/portable/Renesas/RX600"/*.pp "FreeRTOS/Source/portable/Renesas/RX600"/*.mrc "FreeRTOS/Source/portable/Renesas/RX600"/*.prn
	-rm -rf "FreeRTOS/Source/portable/MemMang"/*.obj "FreeRTOS/Source/portable/MemMang"/*.d "FreeRTOS/Source/portable/MemMang"/*.src "FreeRTOS/Source/portable/MemMang"/*.p "FreeRTOS/Source/portable/MemMang"/*.pp "FreeRTOS/Source/portable/MemMang"/*.mrc "FreeRTOS/Source/portable/MemMang"/*.prn
	-rm -rf ".metadata/.plugins/org.eclipse.cdt.make.core"/*.obj ".metadata/.plugins/org.eclipse.cdt.make.core"/*.d ".metadata/.plugins/org.eclipse.cdt.make.core"/*.src ".metadata/.plugins/org.eclipse.cdt.make.core"/*.p ".metadata/.plugins/org.eclipse.cdt.make.core"/*.pp ".metadata/.plugins/org.eclipse.cdt.make.core"/*.mrc ".metadata/.plugins/org.eclipse.cdt.make.core"/*.prn
	@echo.
	@echo 'Clean complete.'

post-build:
	@echo.
	-C:\Renesas\E2_STU~2\DEBUGC~1\RX\RX_CON~1.EXE MT01_proj.abs MT01_proj.x
	@echo.

.PHONY: all clean dependents $(LINKER_OUTPUT_OUTPUTS)
.SECONDARY: post-build

-include ../makefile.targets
