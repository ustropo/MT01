PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_s12ad_rx
==========

Document Number 
---------------
N/A

Version
-------
v1.30

Overview
--------------------------------------------------------------------------------
The r_s12ad_rx module is an A/D driver for the S12ADa/b peripherals. The API 
includes functions to initialize the peripheral, trigger conversions, check for
conversion completion, and read the results. The driver supports all channels
and sensors available on the mcu. The driver can be reduced in size by removing 
the code used for parameter checking. The configuration option for this can be 
found in "r_config\r_s12ad_rx_config.h". An original copy of the configuration 
file is stored in "r_s12ad_rx\ref\r_s12ad_rx_config_reference.h".


Features
--------
* Single Scans and Continuous Scanning.
* Software, asynchronous, and synchronous triggers.
* Group and Double Trigger Modes (on applicable MCUs).
* Automatic addition of multiple samples.


Supported MCUs
--------------
* RX110 Group
* RX111 Group
* RX210 Group
* RX630 Group
* RX631 Group
* RX63N Group


Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX210
* RDKRX63N


Limitations
-----------
Driver does not support RX63T family.


Peripherals Used Directly
-------------------------
* S12ADa
* S12ADb

Required Packages
-----------------
* r_bsp     v2.50


How to add to your project
--------------------------
* Add the r_s12ad_rx and r_config folders to your project.
* Add a project include path for the 'r_s12ad_rx' directory. 
* Add a project include path for the 'r_s12ad_rx\src' directory.
* Add a project include path for the 'r_config' directory.
* Open "r_config\r_s12ad_rx_config.h" file and configure the driver for your 
  project.
* Add a #include for r_s12ad_rx_if.h to any source files that need to use the 
  API functions.


Toolchain(s) Used
-----------------
* Renesas RX v2.01


File Structure
--------------
r_s12ad_rx
|   readme.txt
|   r_s12ad_rx_if.h
|
+---doc
|       r01an1666eu0130_rx.pdf
|
+---ref
|       r_s12ad_rx_config_reference.h
|
+---src
    +-- r_s12ad_rx.c
    +-- r_s12ad_rx_private.h
    |
    +-- targets
        +-- rx110_rx111
        |   +-- r_s12ad_rx110_rx111.c
        |   +-- r_s12ad_rx110_rx111_if.h
        |
        +-- rx210
        |   +-- r_s12ad_rx210.c
        |   +-- r_s12ad_rx210_if.h
        |
        +-- rx63x
            +-- r_s12ad_rx63x.c
            +-- r_s12ad_rx63x_if.h
   
r_config
    r_s12ad_rx_config.h

