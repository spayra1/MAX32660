# max32660_lm75a

### MAX32660 LM75A Library (Experimental)

This repository has an experimental codesets running on MAX32660-EVKIT accessing for LM75A Temperature Sensor board (CJMCJ-75) through I2C protocol(bus). I developed the codes using Maxim Integrated EV Kit Software, Low Power ARM Micro Toolchain(windows) which you can download from following link:

https://www.maximintegrated.com/en/design/software-description.html/swpart=SFW0001500A

Basically, it is Eclipse Neon with GNU ARM Toolchain (on Windows) and board specific libary source codes.

Note: This code set is NOT completed yet. However, you can use this code as a starting place on how to use maxim i2c library.

Right now, it has only basic functionality to get the current temperature every 3 seconds CJMCJ-75 board. 

# Development Platform Information: 

### MAX32660-EVKIT (Evaluation Kit for the MAX32660)

https://www.maximintegrated.com/en/products/microcontrollers/MAX32660-EVKIT.html
https://www.maximintegrated.com/en/products/microcontrollers/MAX32660.html
https://os.mbed.com/platforms/MAX32625PICO/
https://os.mbed.com/teams/MaximIntegrated/wiki/MAX32625PICO-Firmware-Updates

* MAX32660-EVKIT Board
* MAX3265PICO Debug Adapter
* SWD ribbon cable
* USB A to Micro B cable


# FTDI D2XX Programming

http://www.ftdichip.com/Support/Documents/DataSheets/Modules/DS_UMFT201_220_230XB.pdf

###  FTDI UMFT201XB, I2C Slave USB Dongle or  FTDI’s FT201XQ Breakout Board

There is I2C sample code comes with i2c library in the maxim toolchains. However, I wasn't able start immeidately. So, I decided to use FTDI FT201XQ breakout board with D2xx library. With this breakout board, I was able to power the max32660 board and get immediately result interactively what were sent/recived through I2C bus. I will add more on my experience later. One can power (3.3V) MAX32660kEvKit through MAX3265PICO Debug Adapter also (but you need to change the default factory firmware "max32625pico_daplink.bin" with "max32625pico_max32660_power_out.bin", which applies 3.3V to VIO of MAX32660EvKit! For details, please refer to above link.

## Things To Do

* First of all, I need to figure out more on asynchorous read/wirte function in MAXIM I2C library.
* Figure out how to use SPI library of MAXIM library. 
* Figure out how to put the MAX32660 into SLEEP and/or DEEPSLEEP mode in-between the measurement to minimize power consumption.
* My target project is to make a simple temperature-sensor-equipped LoRaWAN end-device.

Note: You can freely use my code if you want even though it is not so much valuable for the experts. Right now I want to define myself as hobbyst, so my code is far from the professional level on this topics.  You can always give me an advice and/or suggestion on my codes and my  target project.
