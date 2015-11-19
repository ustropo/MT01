r_bsp Package
=============

Document Number
---------------
R01AN1685EU

Version
-------
v2.80

Overview
--------
The r_bsp package provides a foundation for code to be built on top of. It provides startup code, iodefines, and MCU
information for different boards. There are 2 folders that make up the r_bsp package. The 'mcu' folder contains files 
that are common to a MCU group. These files provide functionality such as easy register access, CPU functions,
and a file named 'mcu_info.h' for each MCU group. The 'mcu_info.h' file has information about the MCU on the board
and is configured based on the information given in r_bsp_config.h. The information in 'mcu_info.h' is used to help 
configure Renesas middleware that uses the r_bsp package. The 'board' folder has a folder with startup code for each 
supported board.  Which MCU and board is chosen is decided by the settings in 'platform.h'. The user can choose which 
board they are using by uncommenting the include path that applies to their board. For example, if you are using the 
RSK+RX62N then you would uncomment the #include "./board/rskrx62n/r_bsp.h" include path. Users are encouraged to add 
their own boards to the 'board' directory. BSPs are configured by using the r_bsp_config.h file. Each board will have a 
reference configuration file named r_bsp_config_reference.h. The user should copy this file to their project, rename it 
to r_bsp_config.h, and use the options inside the file to configure the BSP for their project.


Features
--------
* Provides foundation to build code on top of.
* Provides MCU startup code.
* Provides SFR access through iodefine.h
* Stores details of MCU in 'mcu_info.h' to help configure Renesas middleware.
* Easily configure BSP through r_bsp_config.h.
* Choose MCU easily by inputting part number details in r_bsp_config.h.
* Provides callbacks for MCU exceptions and the bus error interrupt.
* Supports initializing non-bonded out pins to reduce power
* Provides API to control CPU functions such as setting the IPL, enabling/disabling interrupts, and controlling 
  register protection
 
Supported MCUs
--------------
* RX110 Group
* RX111 Group
* RX113 Group
* RX210 Group
* RX21A Group
* RX220 Group
* RX610 Group
* RX621, RX62N Group
* RX62T Group
* RX62G Group
* RX630 Group
* RX631, RX63N Group
* RX63T Group 
* RX64M Group
* RX71M Group

Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX113
* RPBRX111
* RSKRX210
* RSKRX220
* HSBRX21AP
* RSKRX610
* RSKRX62N
* RSKRX62T
* RSKRX62G
* RDKRX62N
* RSKRX630
* RSKRX63N
* RDKRX63N
* RSKRX63T_64PIN
* RSKRX63T_144PIN
* RSKRX64M
* RSKRX71M
 
Limitations
-----------
N/A

Peripherals Used Directly
-------------------------
N/A

Required Packages
-----------------
N/A

How to add to your project
--------------------------
* Copy the r_bsp folder to your project.
* Add an include path to the 'r_bsp' directory. 
* Add all of the source files for your board from the 'r_bsp\board\--YOUR_BOARD--' directory to your project. 
* Add all of the source files for your MCU group from the 'r_bsp\mcu\--YOUR_MCU_GROUP--' directory to your project. 
* Add all of the source files for your MCU group from the 'r_bsp\mcu\all' directory to your project.
* Uncomment the include path for your board in 'platform.h' which is located in the 'r_bsp' directory.
* Copy the file r_bsp_config_reference.h from the 'r_bsp\board\--YOUR_BOARD--' directory and copy it to your project's
  source code directory. Rename the file r_bsp_config.h.
* Open r_bsp_config.h and use the macros to configure the BSP for your project.

Toolchain(s) Used
-----------------
* Renesas RX v1.02
* Renesas RX v2.00
* Renesas RX v2.01

File Structure
--------------
r_bsp
|   platform.h 
|   readme.txt
|
+---board
|   +---hsbrx21ap
|   +---rdkrx62n
|   +---rdkrx63n
|   +---rpbrx111
|   +---rskrx110
|   +---rskrx111
|   +---rskrx113
|   +---rskrx210
|   +---rskrx220
|   +---rskrx610
|   +---rskrx62g
|   +---rskrx62n
|   +---rskrx62t
|   +---rskrx630
|   +---rskrx63n
|   +---rskrx63t_144pin
|   +---rskrx63t_64pin
|   +---rskrx64m
|   +---rskrx71m
|   \---user
|
+---doc
|       r01an1685eu0280_rx.pdf
|
\---mcu
    +---all
    +---rx110
    +---rx111
    +---rx113
    +---rx210
    +---rx21a
    +---rx220
    +---rx610
    +---rx62g
    +---rx62n
    +---rx62t
    +---rx630
    +---rx63n
    +---rx63t
    +---rx64m
    \---rx71m