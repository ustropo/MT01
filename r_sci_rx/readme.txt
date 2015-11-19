PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_sci_rx
========

Document Number 
---------------
R01AN1815EU0140


Version
-------
v1.40


Overview
--------------------------------------------------------------------------------
The r_sci_rx module is a multi-channel, multi-mode, interrupt-driven driver which
supports Asynchronous, Master Synchronous, and Single Master Simple SPI (SSPI)
operation for the SCIc,d,e and f peripherals. The API includes standard functions 
to initialize a channel and to send and receive data, as well as a special control 
function for taking actions such as issuing a break signal or enabling noise 
cancellation. The driver supports all channels available on the mcu. The driver 
can be reduced in size by removing code used for parameter checking, unused 
channels, or unused modes. These configuration options can be found in 
"r_config\r_sci_rx_config.h". An original copy of the configuration file 
is stored in "r_sci_rx\ref\r_sci_rx_config_reference.h".


Features
--------
* Simultaneous operation of up to 13 channels.
* Simultaneous operation of Async, Sync, or SSPI modes on different channels.
* Queueing of incoming and outgoing data for Asynchronous channels.
* Interrupt driven.


Supported MCUs
--------------
* RX110 Group
* RX111 Group
* RX113 Group
* RX210 Group
* RX63N Group
* RX631 Group


Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX113
* RSKRX210
* RDKRX63N


Limitations
-----------
N/A


Peripherals Used Directly
-------------------------
* SCIc, d, e, or f


Required Packages
-----------------
* r_bsp     v2.80
* r_byteq   v1.0


How to add to your project
--------------------------
* Add the r_sci_rx and r_config folders to your project.
* Add a project include path for the 'r_sci_rx' directory. 
* Add a project include path for the 'r_sci_rx\src' directory.
* Add a project include path for the 'r_config' directory.
* Open "r_config\r_sci_rx_config.h" file and configure the driver for your 
  project.
* Add a #include for r_sci_rx_if.h to any source files that need to use 
  the API functions.


Toolchain(s) Used
-----------------
* Renesas RX v2.01


File Structure
--------------
r_sci_rx
|   readme.txt
|   r_sci_rx_if.h
|
+---doc
|       r01an1815eu0140_rx.pdf
|
+---ref
|       r_sci_rx_config_reference.h
|
+---src
        r_sci_rx.c
        r_sci_rx_private.h
   
r_config
    r_sci_rx_config.h

