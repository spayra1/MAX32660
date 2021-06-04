# MAX32660
Development using the Maxim MAX32660 to serve as an in-fiber microcontroller.

Part of research work with the Fibers@MIT Lab, Professor Yoel Fink, MIT Research Laboratory of Electronics.

Some code originally sourced from Maxim tutorials. This repository is not officially affiliated with Maxim Integrated.

See @ReflexArm repository for other work related to digital fibers

## Goals / Content
* I2C Controller/Client functionality for multi-device communications
* Interfacing the Maxim MAX32660 with Microchip Technologies 24LC128 EEPROM memory
* Interfacing the Maxim MAX32660 with NXP LM75A Temperature Sensor
* Interfacing the Maxim MAX32660 with Memsic MXC4005xC Accelerometer Sensor
* Establishing the Maxim MAX32660 as an LED controller for interactive GPIO indicators
* Interconnecting Maxim MAX32660 units for low-power distributed computing

## Errata
Couple notes on things that took a lot of time and frustration to figure out :)
* Maxim UG7159 has instructions for in-application programming (over I2C/UART/SPI) using the MAX32630FTHR
* If you would like to use the MAX32625 to reprogram a MAX32660 over SWD, the user guide misleadingly suggests that the MAX32625 SWD pinouts are also live on the DIP pins. This is false. Use an SWD ribbon cable and then an SWD breakout to expose pin connectors.
* [This article](https://maximsupport.microsoftcrmportals.com/en-us/knowledgebase/article/KA-03553) contains instructions on how to hard-reset a MAX32660 chip if you've accidentally bricked it by incorrectly assigning pinouts, such as reallocating SWDIO/SWDCLK to other functionalities, and now you can't reprogram your board
* If you're getting an inscrutable "SIGTRAP:Trace/breakpoint trap" error with "bkpt 0x0000" at the address 0x20005094, there is possibly an issue with your RST line. Was encountering this on multiple flex-PCBs; have been able to reproduce this by manually disconnecting the RST line on a functional flex-PCB.
* I've worked out a way to reprogram without a connection to the RST: you can wire the reset signal to a MOSFET and toggle the supply voltage, thereby achieving a power-on reset (POR) and allowing you to flash applications through the bootloader.
