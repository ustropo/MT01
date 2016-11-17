PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_lvd_rx
=========

Document Number 
---------------
R01AN1726EU0150

Version
-------
v1.50

Overview
--------
This module provides an interface to configure the Low Voltage Detection Circuit 
on the RX110, RX111, RX113, RX210, RX231, RX63N, RX64M, and RX71M.

Features
--------
* Configure Low Voltage Detection circuit
* Use to generate NMI, Interrupt, Reset or Polling.
* Supports multiple channels

Supported MCUs
--------------
* RX110 Group
* RX111 Group
* RX113 Group
* RX210 Group
* RX231 Group
* RX63N Group
* RX64M Group
* RX71M Group

Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX113
* RSKRX210
* RSKRX231
* RSKRX63N
* RSKRX64M
* RSKRX71M

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* LVD

Required Packages
-----------------
* r_bsp_rx     v2.30         For RX111, RX210, RX63N
* r_bsp_rx     v2.50         For RX110
* r_bsp_rx     v2.70         For RX113
* r_bsp_rx     v2.80         For RX64M, 71M
* r_bsp_rx     v2.90         For RX231

How to add to your project
--------------------------
* Add src\r_lvd_rx.c to your project.
* Add an include path to the 'r_lvd_rx' directory. 
* Add an include path to the 'r_lvd_rx\src' directory.
* Copy the reference configuration file 'r_lvd_rx_config_reference.h' to your project and rename it r_lvd_rx_config.h.
* Configure middleware for your system through just copied r_lvd_config.h.
* Add a #include for r_lvd_rx_if.h to any source files that need to use the API functions.

Toolchain(s) Used
-----------------
* Renesas RX v2.02

File Structure
--------------
r_lvd_rx
|   readme.txt
|   r_lvd_rx_if.h
|
+---doc
|       r01an1726eu0150_rx.pdf
|
+---ref
|       r_lvd_rx_config_reference.h
|
+---src
        r_lvd_rx.c
        r_lvd_rx.h




