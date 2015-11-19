PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_dtc_rx
=========

Document Number 
---------------
R01AN1819EJ0202

Version
-------
v2.02

Overview
--------
The DTC driver provides a method to transmit the data using Data Transfer Controller (DTC).
The driver includes API functions to initialize DTC, create Transfer data, Control and get status of DTC.
The driver can be reduced in size by removing code used for parameter checking. 
All configuration options can be found in "r_config\r_dtc_rx_config.h". 
An original copy of the configuration file is stored in "r_dtc_rx\ref\r_dtc_rx_config_reference.h".

Features
--------
* Support Normal trasnsfer mode, Repeat trasnsfer mode and Block trasnsfer mode.
* Support chain transfer

Supported MCUs
--------------
* RX110 MCU
* RX111 MCU
* RX113 MCU
* RX64M MCU
* RX71M MCU

Boards Tested On
----------------
* RSKRX110
* RSKRX111
* RSKRX113
* RSKRX64M
* RSKRX71M

Limitations
-----------
* None

Peripherals Used Directly
-------------------------
* Data Transfer Controller (DTC)

Required Packages
-----------------
* r_bsp v2.80

How to add to your project
--------------------------
The FIT module must be added to each project in the e2Studio.
You can use the FIT plug-in to add the FIT module to your project, or the module can be added manually.
It is recommended to use the FIT plug-in as you can add the module to your project easily and also it will 
automatically update the include file paths for you.
To add the FIT module using the plug-in, refer to "Adding FIT Modules to e2 studio Projects Using FIT Plug-In" in the 
"Adding Firmware Integration Technology Modules to Projects" application note (R01AN1723EU).
When using the FIT module, the BSP FIT module also needs to be added. For details on the BSP FIT module, refer to the 
"Board Support Package Module Using Firmware Integration Technology" application note (R01AN1685EU).
To add the FIT module manually, refer to the following steps.

 1. This application note is distributed with a zip file package that includes the DTC FIT module in its own folder
    r_dtc_rx.
 2. Unzip the package into the location of your choice.
 3. In a file browser window, browse to the directory where you unzipped the distribution package and locate the
    r_dtc_rx folder.
 4. Open your e2Studio workspace.
 5. In the e2Studio project explorer window, select the project that you want to add the DTC FIT module to.
 6. Drag and drop the r_dtc_rx folder from the browser window into your e2Studio project at the top level of the
    project.
 7. Update the source search/include paths for your project by adding the paths to the module files:
    a. Navigate to the "Add directory path" control:
       i. 'project name' -> properties -> C/C++ Build -> Settings -> Compiler -> Source -Add (green + icon)
    b. Add the following paths:
       i. "${workspace_loc:/${ProjName}/r_dtc_rx}"
    Whether you used the plug-in or manually added the package to your project, it is necessary to configure the module
    for your application.
 8. Locate the r_dtc_rx_config_reference.h file in the r_dtc_rx/ref source folder in your project and copy it to the
    r_config folder in your project.
 9. Change the name of the copy in the r_config folder to r_dtc_rx_config.h.
10. Make the required configuration settings by editing the r_dtc_rx_config.h file. Refer to 2.6 "Compile Settings" for
    details.

Toolchain(s) Used
-----------------
* Renesas RX v2.01

File Structure
--------------
r_dtc_rx
|   r_dtc_rx_if.h
|   readme.txt
|
+---doc
|       r01an1819ej0202_rx.pdf
|
+---ref
|       r_dtc_rx_config_reference.h
|
+---src
    |   r_dtc_rx.c
    |   r_dtc_rx_private.h
    |
    +---targets
        |
        +---rx64m
        |       r_dtc_rx_target.c
        |       r_dtc_rx_target.h
        |
        +---rx71m
        |       r_dtc_rx_target.c
        |       r_dtc_rx_target.h
        |
        +---rx110
        |       r_dtc_rx_target.c
        |       r_dtc_rx_target.h
        |
        +---rx111
        |        r_dtc_rx_target.c
        |        r_dtc_rx_target.h
        |
        +---rx113
                r_dtc_rx_target.c
                r_dtc_rx_target.h

r_config
    r_dtc_rx_config.h


