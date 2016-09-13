# Arduino-Eprom-Writer
An EPROM reader/writer for Arduino

These two sketches read/write type 2532 4k and 2516 2k UV-erasable EPROMs. It was developed to troubleshoot/replace ROMs in an old arcade machine.

## Reading data
Data output as hex dump or Intel Hex file format

Insert EPROM into the socket and power up the Arduino
Once the sketch is uploaded and the device is initialised, LED 1 will flash.
Then open the serial monitor and the EPROM contents will be read out and dumped over the serial monitor
Comment/uncomment dumpRomText() and dumpRomIntelHex() to select desired output format
Comment/uncomment the appropriate 'for' loop to read 4k or 2k ROMs

## Writing data
Data to write is placed in a static array in 'image.h' - use bin2c to convert binary files to a 'C' array compatible with this sketch.
Set the VPP switch to 5V before powering up
Once LED1 and LED2 are flashing alternately open the serial monitor and follow the instrictions to start programming
When finished, switch VPP back to 5V and disconnect the Arduino

Circuit diagram to follow...
