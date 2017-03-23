PLEASE REFER TO THE APPLICATION NOTE FOR THIS DRIVER FOR MORE INFORMATION

r_sci_async_rx
==============

Document Number 
---------------
N/A

Version
-------
v2.12

Overview
--------------------------------------------------------------------------------
The r_sci_async_rx module is a multi-channel interrupt-driven asynchronous 
(UART) driver for the SCIc,d,e and f peripherals. The API includes functions to
initialize a channel and to send and receive data. A special control function is 
available for taking actions such as issuing a break signal or enabling noise 
cancellation. The driver supports all channels available on the mcu. The driver 
can be reduced in size by removing code used for parameter checking or for 
unused channels. These configuration options can be found in "r_config\
r_sci_async_rx_config.h". An original copy of the configuration file is stored in 
"r_sci_async_rx\ref\r_sci_async_rx_config_reference.h".


Features
--------
* Simultaneous operation of up to 13 channels.
* Queueing of incoming and outgoing data.
* Interrupt driven.


Supported MCUs
--------------
* RX111 Group
* RX210 Group
* RX63N Group
* RX631 Group


Boards Tested On
----------------
* RSKRX111
* RSKRX210
* YRDKRX63N


Limitations
-----------
N/A


Peripherals Used Directly
-------------------------
* SCIc, d, e, or f


Required Packages
-----------------
* r_bsp     v2.10
* r_byteq   v1.0


How to add to your project
--------------------------
* Add the r_sci_async_rx and r_config folders to your project.
* Add a project include path for the 'r_sci_async_rx' directory. 
* Add a project include path for the 'r_sci_async_rx\src' directory.
* Add a project include path for the 'r_config' directory.
* Open "r_config\r_sci_async_rx_config.h" file and configure the driver for your 
  project.
* Add a #include for r_sci_async_rx_if.h to any source files that need to use 
  the API functions.


Toolchain(s) Used
-----------------
* Renesas RX v1.02


File Structure
--------------
r_sci_async_rx
|   readme.txt
|   r_sci_async_rx_if.h
|
+---doc
|       R01AN1667EU0200_RX.pdf
|
+---ref
|       r_sci_async_rx_config_reference.h
|
+---src
        r_sci_async_rx.c
        r_sci_async_rx_private.h
   
r_config
    r_sci_async_rx_config.h

