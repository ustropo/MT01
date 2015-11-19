PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_usb_basic
=======================

Document Number 
---------------
R01AN2025EJ

Version
-------
v1.10

Overview
--------
USB Basic Host and Peripheral firmware

Features
--------
* Can operate in either host or peripheral mode.
* Device connect/disconnect, suspend/resume, and USB bus reset processing.
* Control transfer on pipe 0.
* Data transfer on pipes 1 to 9. (bulk or interrupt transfer: CPU access/DTC or DMA access)

Supported MCUs
--------------
* RX64M Group
* RX71M Group

Boards Tested On
----------------
* RSKRX64M
* RSKRX71M
 
Limitations
-----------

Peripherals Used Directly
-------------------------


Required Packages
-----------------
* r_bsp
* r_usb_dtc (using DTC transfer)

How to add to your project
--------------------------

Toolchain(s) Used
-----------------
* Renesas RX v.2.01.00

File Structure
--------------
r_usb_basic
|   readme.txt
|   r_usb_basic_if.h
|
+---doc
|       r01an2025ej0110_usb.pdf
|
+---ref
|       r_usb_config_reference.h
|
\---src
     +---driver
     |     +---comm
     |     |      r_usb_cdataio.c
     |     |      r_usb_cintfifo.c
     |     |      r_usb_cinthandler_usbip0.c
     |     |      r_usb_cinthandler_usbip1.c
     |     |      r_usb_clibusbip.c
     |     |      r_usb_cscheduler.c
     |     |      r_usb_cstdapi.c
     |     |      r_usb_cstdfunction.c
     |     |
     |     +---host
     |     |      r_usb_hbc.c
     |     |      r_usb_hcontrolrw.c
     |     |      r_usb_hdriver.c
     |     |      r_usb_hdriverapi.c
     |     |      r_usb_hhubsys.c
     |     |      r_usb_hintfifo.c
     |     |      r_usb_hlibusbip.c
     |     |      r_usb_hmanager.c
     |     |      r_usb_hsignal.c
     |     |      r_usb_hstdfunction.c
     |     |      
     |     +---peri
     |     |      r_usb_pbc.c
     |     |      r_usb_pcontrolrw.c
     |     |      r_usb_pdriver.c
     |     |      r_usb_pdriverapi.c
     |     |      r_usb_pintfifo.c
     |     |      r_usb_psignal.c
     |     |      r_usb_pstdfunction.c
     |     |      r_usb_pstdrequest.c
     |     |
     |     \---inc
     |            r_usb_api.h
     |            r_usb_cdefusbip.h
     |            r_usb_cextern.h
     |            r_usb_ckernelid.h
     |            r_usb_cmacprint.h
     |            r_usb_cmacsystemcall.h
     |            r_usb_ctypedef.h
     |            
     \---HW
           +---comm
           |      r_usb_creg_abs.c
           |      r_usb_creg_access.c
           |      r_usb_creg_dmadtc.c
           |      rx_mcu.c
           |
           +---host
           |      r_usb_hostelectrical.c
           |      r_usb_hreg_abs.c
           |      r_usb_hreg_access.c
           |
           +---peri
           |      r_usb_preg_abs.c
           |      r_usb_preg_access.c
           |
           \---inc
                  r_usb_cusb_bitdefine.h
                  r_usb_defvalue.h
                  r_usb_fixed_config.h
                  r_usb_reg_access.h
                  r_usb_sysdef.h
                  rx_rsk_extern.h
