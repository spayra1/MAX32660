# MAX32660
Development using the Maxim MAX32660 microcontroller

## Content
* I2C Controller/Client functionality for multi-device communications
* Interfacing the Maxim MAX32660 with Microchip Technologies 24LC128 EEPROM memory
* Interfacing the Maxim MAX32660 with NXP LM75A Temperature Sensor
* Interfacing the Maxim MAX32660 with Memsic MXC4005xC Accelerometer Sensor
* Establishing the Maxim MAX32660 as an LED controller for interactive GPIO indicators
* Interconnecting Maxim MAX32660 units for low-power distributed computing

## Goals
Development to enable the MAX32660 to serve as an in-fiber microcontroller.
Part of research work with the Fibers@MIT Lab, Professor Yoel Fink, MIT Research Laboratory of Electronics
See @ReflexArm repository for other work related to digital fibers

## Errata
Couple notes on things that took a lot of time and frustration to figure out. I may reallocate these in the future:
* Maxim UG7159 has instructions for in-application programming (over I2C/UART/SPI) using the MAX32630FTHR
* [This article](https://maximsupport.microsoftcrmportals.com/en-us/knowledgebase/article/KA-03553) contains instructions on how to hard-reset a MAX32660 chip if you've accidentally bricked it by incorrectly assigning pinouts, such as reallocating SWDIO/SWDCLK to other functionalities, and now you can't reprogram your board
* If you're getting an inscrutable "SIGTRAP:Trace/breakpoint trap" error with "bkpt 0x0000" at the address 0x20005094, there is possibly an issue with your RST line. Was encountering this on multiple flex-PCBs; have been able to reproduce this by manually disconnecting the RST line on a functional flex-PCB.
