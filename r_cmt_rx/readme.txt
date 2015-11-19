PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

Document Number 
---------------
R01AN1856EU0241

Version
-------
v2.41

Overview
--------
This module creates a timer tick using a CMT channel based on a frequency input by the user.

Features
--------
* Create periodic or one-shot timer easily by passing in desired frequency/period
* User is alerted through callback function
* CMT channels are allocated dynamically.

Supported MCUs
--------------
* RX621, RX62N Group
* RX62T Group
* RX630 Group
* RX631, RX63N Group 
* RX210 Group
* RX110 Group
* RX111 Group
* RX113 Group
* RX64M Group


Boards Tested On
----------------
* RSKRX62T
* RDKRX62N
* RSKRX630
* RSKRX63N
* RDKRX63N
* RSKRX210
* RSKRX110
* RSKRX111
* RSKRX113
* RSKRX64M

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* CMT

Required Packages
-----------------
* None

How to add to your project
--------------------------
* Add src\r_cmt_rx.c to your project.
* Add an include path to the 'r_cmt_rx' directory. 
* Add an include path to the 'r_cmt_rx\src' directory.
* Copy r_cmt_rx_config_reference.h from 'ref' directory to your desired location and rename to r_cmt_rx_config.h.
* Configure middleware through r_cmt_rx_config.h.
* Add a #include for r_cmt_rx_if.h to any source files that need to use the API functions.

Toolchain(s) Used
-----------------
* Renesas RX v2.02

File Structure
--------------
r_cmt_rx
|   readme.txt
|   r_cmt_rx_if.h
|
+---doc
|       r01an1856eu0241_rx.pdf
|
+---ref
|       r_cmt_rx_config_reference.h
|
\---src
        r_cmt_rx.c
        
r_config
    r_cmt_rx_config.h
            
                
