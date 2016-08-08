# Arduino-Eprom-Writer
An EPROM reader/writer for Arduino

These two sketches read/write type 2532 4k UV-erasable EPROMs. It was developed to replace a ROM in an old arcade machine.

## Reading data
Data output as hex dump or Intel Hex file format

Insert EPROM into the socket and power up the Arduino
Once the sketch is uploaded open the serial monitor
The EPROM contents will be read out and dumped over the serial monitor
Comment/uncomment dumpRomText() and dumpRomIntelHex() to select desired output format

## Writing data
Data to write is placed in a static array in the sketch
Set the VPP switch to 5V before powering up
Once LEDx lights up, set VPP switch to select the programming voltage
Open the serial monitor to start programming
When finished, switch VPP back to 5V and disconnect the Arduino

Circuit diagram to follow...
