PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

Simple Flash API for RX
=======================

Document Number 
---------------
R01AN0544EU0240

Version
-------
v2.40

Overview
--------
A simple Application Program Interface (API) has been created to allow users of flash based RX600 & RX200 Series devices
to easily integrate reprogramming abilities into their applications using User Mode programming. User Mode programming 
is the term used to describe a Renesas MCU's ability to reprogram its own internal flash memory while running in its 
normal operational mode. 

Features
--------
* Can write/erase any block of internal ROM.
* Can write/erase any block of data flash.
* Supports background operations (BGO) on ROM and data flash.
* Has callbacks for be alerted when BGO have finished.

Supported MCUs
--------------
* RX610 Group
* RX621, RX62N Group
* RX62T Group
* RX62G Group
* RX630 Group
* RX631, RX63N Group 
* RX63T Group
* RX210 Group

Boards Tested On
----------------
* RSKRX610
* RSK+RX62N
* RSKRX62T
* YRDKRX62N
* RSKRX630
* RSK+RX63N
* YRDKRX63N
* RSKRX62G
* RSKRX63T
* RSKRX210
 
Limitations
-----------
* This code is not re-entrant but does protect against multiple concurrent function calls.
* During ROM operations neither the ROM or DF can be accessed. If using ROM BGO then make sure code runs from RAM.
* During DF operations the DF cannot be accessed but ROM can be accessed normally.

Peripherals Used Directly
-------------------------
* Flash Control Unit (FCU)

Required Packages
-----------------
N/A

How to add to your project
--------------------------
* Add src\r_flash_api_rx.c to your project.
* Add an include path to the 'r_flash_api_rx' directory. 
* Add an include path to the 'r_flash_api_rx\src' directory.
* Copy the reference configuration file 'r_flash_api_rx_config_reference.h' to your project and rename it 
  r_flash_api_rx_config.h.
* Configure middleware for your system through just copied r_flash_api_rx_config.h.
* Add a #include for r_flash_api_rx_if.h to any source files that need to use the Flash API.
* (The following steps are only required if you are programming or erasing ROM. If you are only operating on data 
  flash, then these steps can be ignored. These steps are discussed with more detail in the app note.)
* Make a ROM section named 'PFRAM'.
* Make a RAM section named 'RPFRAM'.
* Configure your linker such that code allocated in the 'FRAM' section will actually be executed in RAM.
* After reset, make sure the Flash API code is copied from ROM to RAM. This can be done by calling the 
  R_FlashCodeCopy() function.

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_flash_api_rx
|   readme.txt
|   r_flash_api_rx_if.h
|
+---demo
|       flash_api_rx_demo_main.c
|
+---doc
|       r01an0544e_rxap.pdf
|
+---ref
|       r_flash_api_rx_config_reference.h
|
\---src
    |   r_flash_api_rx.c
    |   r_flash_api_rx_private.h
    |
    \---targets
        +---rx210
        |       r_flash_api_rx210.h
        |
        +---rx610
        |       r_flash_api_rx610.h
        |
        +---rx62g
        |       r_flash_api_rx62g.h
        |
        +---rx62n
        |       r_flash_api_rx62n.h
        |
        +---rx62t
        |       r_flash_api_rx62t.h
        |
        +---rx630
        |       r_flash_api_rx630.h
        |
        +---rx63n
        |       r_flash_api_rx63n.h
        |
        \---rx63t
                r_flash_api_rx63t.h

