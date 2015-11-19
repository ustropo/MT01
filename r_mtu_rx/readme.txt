PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE, "r01an2329eu0100_rx_family.pdf", FOR MORE INFORMATION

r_mtu_rx
=========

Document Number 
---------------
R01AN2329EU0100

Version
-------
v1.00

Overview
--------
This software provides an applications programing interface (API) for using the Multi-Function Timer Pulse Unit 2 
(MTU2a) peripheral of supported RX Family microcontrollers. This API is designed to make it easy to add timing 
operations to your application software by providing a highly abstracted, use-case oriented interface to the 
hardware. Rather than referring to hardware specific register names and internal control bits, this API lets you 
set up your timing operations in terms of ‘what you want to do’. 

Features
--------
* Timer Function and Compare Match or Output Compare 
* Input Capture
* PWM modes 1 and 2

Supported/Tested MCUs
--------------
* RX63N
* RX210
* RX111
* RX110


Boards Tested On
----------------
* RDKRX63N
* RSKRX210
* RSKRX110
* RSKRX111


Limitations
-----------
* MTU2a channel 5 not supported

Peripherals Used Directly
-------------------------
* MTU2a

Required Packages
-----------------
* r_bsp     v2.50 or higher


How to add to your project
--------------------------
* Add folder "r_mtu_rx\" to your project.
*...

* Copy the reference configuration file 'r_mtu_rx_config_reference.h' to your project and rename it r_mtu_rx_config.h.
* Configure the MTU module options for your system using the just copied r_mtu_rx_config.h.
* Add a #include for r_mtu_rx_if.h to any source files that need to use the API functions.

Toolchain(s) Used
-----------------
* Renesas RX v2.0x

File Structure
--------------
r_mtu_rx
|   readme.txt
|   r_mtu_rx_if.h
|
+---doc
|       r01an2329eu0100_rx_family.pdf
|
+---ref
|       r_mtu_rx_config_reference.h
|
+---src
|       r_mtu_rx_common.c
|       r_mtu_timers_rx.c
|       r_mtu_pwm_rx.c
|       r_mtu_rx_private.h




