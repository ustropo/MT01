
r_byteq
=======

Document Number 
---------------
R01AN1683EU0140

Version
-------
v1.40

Overview
--------------------------------------------------------------------------------
The r_byteq module is a collection of circular buffer routines for byte data.
The application passes a buffer to be used as a circular buffer to the Open() 
function which assigns a queue control block to it to handle indexing. The 
Open() function returns a handle which is then used as a queue/buffer id for all 
other API functions. These functions include routines for adding and removing 
data from a queue, inspecting the amount of data in a queue, and the ability to 
flush a queue.

The queue control blocks can be allocated at compile time or dynamically at run
time. A configuration option for this exists in "r_config\r_byteq_config.h".
An original copy of the configuration file is stored in "r_byteq\ref\
r_byteq_config_reference.h".


Features
--------
* Statically or dynamically allocated queue control blocks.
* Number of queues limited only by the amount of RAM available on the mcu.


Supported MCUs
--------------
N/A


Boards Tested On
----------------
N/A


Limitations
-----------
* For byte data only.


Peripherals Used Directly
-------------------------
N/A


Required Packages
-----------------
* None


How to add to your project
--------------------------
* Add the r_byteq and r_config folders to your project.
* Add a project include path for the 'r_byteq' directory. 
* Add a project include path for the 'r_byteq\src' directory.
* Add a project include path for the 'r_config' directory.
* Open "r_config\r_byteq_config.h" file and configure the driver for your 
  project.
* Add a #include for r_byteq_if.h to any source files that need to use the 
  API functions.


Toolchain(s) Used
-----------------
* Renesas RX v2.02


File Structure
--------------
r_byteq
|   readme.txt
|   r_byteq_if.h
|
+---doc
|       r01an1683eu0140_rx.pdf
|
+---ref
|       r_byteq_config_reference.h
|
+---src
        r_byteq.c
        r_byteq_private.h
   
r_config
    r_byteq_config.h

