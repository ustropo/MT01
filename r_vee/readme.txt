PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

Renesas Virtual EEPROM for RX
=============================

Document Number 
---------------
R01AN0724EU0170

Version
-------
v1.70

Overview
--------
The Virtual EEPROM project allows user to use their data flash as they would an EEPROM

Features
--------
* Can write with 1-byte granularity no matter what the actual granularity of your data flash is
* Wear leveling on data flash to increase longevity
* Read/Write by sending in a structure to API
* Use background operation (BGO) functionality of MCUs so calls do not block user application
* Configurable vEE Sectors if you want to separate records (e.g. separate often-written records from not often-written)
* Automatically recovers from reset/power down during programs & erases
* Adapts to different types of data flash

Supported MCUs
--------------
* RX621, RX62N Group
* RX62T Group
* RX62G Group
* RX630 Group
* RX631, RX63N Group
* RX63T Group
* RX210 Group

Boards Tested On
----------------
* RSKRX62N
* RSKRX62T
* RSKRX62G
* RDKRX62N
* RSKRX630
* RSKRX63N
* RDKRX63N
* RSKRX63T_64PIN
* RSKRX63T_144PIN
* RSKRX210

Limitations
-----------
* Not all functions are re-entrant but the code does protect against multiple concurrent function calls.
* When using this middleware it is recommended that the user reserve the entire data flash for VEE operations. If this 
  is not done then the user will need to take necessary precautions to ensure that their own data flash operations
  do not interfere with VEE data flash operations.
* After performing a VEE read the function R_VEE_ReleaseState() must be called to perform any more VEE operations. This 
  is done to ensure safety during data flash operations.

Peripherals Used Directly
-------------------------
* none

Required Packages
-----------------
* r_flash_api_rx - Simple Flash API for RX (R01AN0544EU)

How to add to your project
--------------------------
* Add r_vee.c to your project
* Add an include path to the 'r_vee' directory 
* Add an include path to the 'r_vee\src' directory  
* Add the source file for your MCU port to your project (e.g. r_vee_rx62x.c)
* Copy r_vee_config_reference.h from 'ref' directory to your desired location and rename to r_vee_config.h.
* Configure middleware through r_vee_config.h.
* Add a #include for r_vee_if.h to any source files that need to use the VEE API

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_vee
|   readme.txt
|   r_vee_if.h
|
+---demo
|       vee_demo_main.c
|
+---doc
|       r01an0724eu0170_rx.pdf
|
+---ref
|       r_vee_config_reference.h
|
\---src
    |   r_vee.c
    |   r_vee.h
    |   r_vee_target.h
    |   r_vee_types.h
    |
    \---targets
        +---rx21x
        |       r_vee_config_rx21x_8kb.h
        |       r_vee_rx21x.c
        |
        +---rx62x
        |       r_vee_config_rx62x_32kb.h
        |       r_vee_config_rx62x_8kb.h
        |       r_vee_rx62x.c
        |
        \---rx63x
                r_vee_config_rx63x_32kb.h
                r_vee_config_rx63x_8kb.h
                r_vee_rx63x.c


