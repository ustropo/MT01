PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_crc_rx
========

Document Number 
---------------
Sample Code

Version
-------
v1.20

Overview
--------
Performs 16-bit CRC calculations using the CRC peripheral on RX MCUs.

Features
--------
* Has options for 3 different polynomials

Supported MCUs
--------------
* RX610 Group
* RX621, RX62N Group
* RX62T Group
* RX630 Group
* RX631, RX63N Group 
* RX63T Group
* RX210 Group
* RX111 Group

Boards Tested On
----------------
* RSKRX610
* RSK+RX62N
* RSKRX62T
* RDKRX62N
* RSKRX630
* RSKRX63N
* RDKRX63N

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* CRC

Required Packages
-----------------
* None

How to add to your project
--------------------------
* Add src\r_crc_rx.c to your project.
* Add an include path to the 'r_crc_rx' directory. 
* Add an include path to the 'r_crc_rx\src' directory.
* Copy r_crc_rx_config_reference.h from 'ref' directory to your desired location and rename to r_crc_rx_config.h.
* Configure middleware through r_crc_rx_config.h.
* Add a #include for r_crc_rx_if.h to any source files that need to use this module.

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_crc_rx
|   readme.txt
|   r_crc_rx_if.h
|
+---doc
+---ref
|       r_crc_rx_config_reference.h
|
\---src
        r_crc_rx.c                
