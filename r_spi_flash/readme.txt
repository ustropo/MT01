PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_spi_flash
===========

Document Number 
---------------
Sample Code

Version
-------
v1.30

Overview
--------
Implements the legacy SPI protocol that is used to read, write, and communicate with a SPI flash. Basic commands are 
supported.

Features
--------
* Supports legacy SPI protocol.
* Takes care of reads and writes for commands.
* Easy to configure for different chips.

Supported MCUs
--------------
* All

Boards Tested On
----------------
* RSK+RX62N
* RSK+RX63N
* RDKRX62N
* RDKRX63N
* RSKRX210

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* None

Required Packages
-----------------
* r_rspi_rx

How to add to your project
--------------------------
* Add src\r_spi_flash.c to your project.
* Add an include path to the 'r_spi_flash' directory. 
* Add an include path to the 'r_spi_flash\src' directory.
* Copy r_spi_flash_config_reference.h from 'ref' directory to your desired location and rename to r_spi_flash_config.h.
* Configure middleware through r_spi_flash_config.h.
* Add a #include for r_spi_flash_if.h to any source files that need to use this module.

Toolchain(s) Used
-----------------
* Renesas RX v1.02

File Structure
--------------
r_spi_flash
|   readme.txt
|   r_spi_flash_if.h
|
+---doc
+---ref
|       r_spi_flash_config_reference.h
|
\---src
    |   r_spi_flash.c
    |
    \---chips
            r_spi_flash_m25p16.h
            r_spi_flash_p5q.h
            r_spi_flash_sst25.h                


