# ft2232_m25p10_programmer

Programmer for SPI flash chip MP25P10. For programming via FTDI FT2232.

Compatible with nandland.com Go board.


NOTE: USE THIS PROGRAM AT YOUR OWN RISK!



Compiling:

Download FTDI LibMPSSE here https://www.ftdichip.com/Support/SoftwareExamples/MPSSE/LibMPSSE-SPI.htm
Extract LibMPSSE
Place this folder inside the subfolder libMPSSE__0.6\samples\SPI\

Note that only debug build seem to work.



Connection:

MP25P10 is a 1 Mb flash memory with SPI interface.

FT2232 is an integrated circuit with USB connection, which can be connected to other devices via SPI.
Assuming this connection:
ADBUS0: SPI clock
ADBUS1: SPI MOSI
ADBUS2: SPI MISO
ADBUS4: SPI SS



Usage:

Connect the FT2232-chip to the computer via USB. Make sure the USB driver is installed.

Start the program. If the library finds a FT2232 chip attached to the computer, it will list the available channels.
Enter the channel number (0/1/...) and enter.
The program will test if the channel is connected to an SPI-based MP25P10-compatible chip. If it fails, try another channel.

The basic menu supports reading and programming.
The program uses two hardcoded filenames:
"input.bin" - This file will be uploaded to the flash. Copy your file to the folder of the executable, and rename it to "input.bin".
"output.bin" - A flash readout will write to this filename. Make sure to rename this file if you want to keep it.