PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

Flash Loader: A Custom Bootloader Framework for RX MCUs
=======================================================

Document Number 
---------------
R01AN0287EU0300

Version
-------
v3.0

Overview
--------
The Flash Loader project is a flexible system thats goal is to help users add the ability to upgrade their firmware 
in-the-field.  The Flash Loader project gives users a framework to build off of so that they can customize the project 
to meet their own specific needs.  

Features
--------
* Framework is modular to make it easy to conform to any system.
* Does not rely on any communications protocols for error detection or retries. All of this is built into the framework.
* Uses the User Boot area on RX MCUs. This area offers several key benefits for bootloaders.
* PC software is supplied to create firmware images that can be sent to the MCU.
* All protocols are documented in the application note.

Supported MCUs
--------------
* RX610 Group
* RX621, RX62N Group
* RX62T Group
* RX630 Group
* RX631, RX63N Group 

Boards Tested On
----------------
* RSK+RX62N
* RDKRX62N
* RSKRX63N
* RDKRX63N

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* WDT

Required Packages
-----------------
* r_cmt_rx
* r_crc_rx
* r_delay (RDKs only)
* r_flash_api_rx
* r_glyph (RDKs only)
* r_rspi_rx
* r_sci_async_1ch_rx
* r_spi_flash

How to add to your project
--------------------------
Flash Loader Bootloader
* Copy the 'r_flash_loader_rx' directory (packaged with this application note) to your project directory.
* Add src\r_fl_bootloader.c to your project.
* Add src\r_fl_downloader.c to your project.
* Add src\r_fl_store_manager.c to your project.
* Add src\r_fl_utilities.c to your project.
* Add the source file from the 'communications' directory that corresponds to your project's method of communication 
  between the Host and Device.
* Add the source file from the 'memory' directory that corresponds to your project's method of communication between the
  Device and Storage.
* Add an include path to the 'r_flash_loader' directory. 
* Add an include path to the 'r_flash_loader\src' directory.
* Copy r_flash_loader_config_reference.h from 'ref' directory to your desired location and rename to 
  r_flash_loader_config.h.
* Configure middleware through r_flash_loader_config.h.
* If you are placing the bootloader in the User Boot area then make sure to:
* Configure your linker to place the code in the correct area.
* Configure your BSP to choose User Boot Mode. This is done by configuring r_bsp_config.h if you are using the 
  r_bsp package.

Flash Loader User Application
* Copy the 'r_flash_loader_rx' directory (packaged with this application note) to your project directory.
* Add src\r_fl_app_header.c to your project.
* Add src\r_fl_downloader.c to your project.
* Add src\r_fl_store_manager.c to your project.
* Add src\r_fl_utilities.c to your project.
* Add the source file from the 'communications' directory that corresponds to your project's method of communication 
  between the Host and Device.
* Add the source file from the 'memory' directory that corresponds to your project's method of communication between the
  Device and Storage.
* Add an include path to the 'r_flash_loader' directory. 
* Add an include path to the 'r_flash_loader\src' directory.
* Copy r_flash_loader_config_reference.h from 'ref' directory to your desired location and rename to 
  r_flash_loader_config.h.
* Configure middleware through r_flash_loader_config.h.
* Add a #include for r_flash_loader_rx_if.h to files that need to use this package. 

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_flash_loader_rx
|   readme.txt
|   r_flash_loader_rx_if.h
|
+---demo
|       flash_loader_user_app_main.c
|
+---doc
|       r01an0287eu0300_rx.pdf
|
+---ref
|       r_flash_loader_rx_config_reference.h
|
+---src
|   |   r_fl_app_header.c
|   |   r_fl_bootloader.c
|   |   r_fl_comm.h
|   |   r_fl_downloader.c
|   |   r_fl_downloader.h
|   |   r_fl_globals.h
|   |   r_fl_includes.h
|   |   r_fl_memory.h
|   |   r_fl_store_manager.c
|   |   r_fl_store_manager.h
|   |   r_fl_types.h
|   |   r_fl_utilities.c
|   |   r_fl_utilities.h
|   |
|   +---communications
|   |       r_fl_comm_uart.c
|   |
|   \---memory
|           r_fl_memory_spi_flash.c
|
\---utilities
    +---batch_files
    |       fl_script_erase_b0.bat
    |       fl_script_erase_b1.bat
    |       fl_script_info.bat
    |       fl_script_load.bat
    |       open_cmd_window.bat
    |
    \---python
            r_fl_mot_converter.py
            r_fl_serial_flash_loader.py
                            



