R2C2 inkjet functions
~~~~~~~~~~~~~~~~~~~~~


This directory contains the information to hook up your inkshield to 
a R2C2 controller board. (www.3dprinting-r2c2.com).

A special firmware is in development to be able to drive the printhead.
A compiled firmware version is supplied in this directory.
Install it on your R2C2 board by activating the bootloader and copying the
file to your board using this procedure:
http://www.3dprinting-r2c2.com/?q=content/r2c2-usb-bootloader


You can download the source for this special R2C2 version
from github: https://github.com/pbrier/R2C2_Firmware

NOTE: Using this software hardware and may damage your inkshield, 
R2C2 board, your printhead! If may also contaminate everything in your surrounding with ink. 
YOU ARE WARNED!


Currently the following commands are implemented:



M700 S[xxxx]
~~~~~~~~~~~~
Set the bitmask to enable/disable nozzles. Where xxx is the decimal value of the bitmask
If the bitmask is "0" no nozzles are jetting, if set to "1" nozzle 1 is jetting, set at "2" nozzle 2 is jetting, 
and set at "3" nozzle 1 + 2 are jetting. Etc. Upto 12 nozzles can be addressed. (S4095 activates all 12 nozzles)



M701 S[xxxx]
~~~~~~~~~~~~
Set the pulse duration. Typically set at 10 (5 usec pulse). Can be used to experiment with the printhead. set between
1 and 15.


The current experimental firmware function can only jet a nozzle in sync with "E" axis movements. 
You can use the "steps_per_mm" value of the E axis and the speed ("F") to control the frequency.
Each nozzle is fired sequentially, so they each jet at 1/12th of the frequency.  

Setting e_steps_per_mm to 1000, and moving with 1 mm/sec will eject 1000 drops/second.
For example:

   M200 E1000  ; set e steps/mm to 1000
   G1 E10 F60  ; move 10mm with 1mm/sec --> 10 seconds movement with 1000 Hz. 

Together with the bitmask function you should be able to make some funky drawings already!

   M200 E1000  ; set e steps/mm to 1000
   M700 S4095  ; enable all nozzles
   G1 X10 E10 F60  ; move 10mm with 1mm/sec --> 10 seconds movement with 1000 Hz.
   G1 X0 Y1        ; Move to next line, no nozzles are activated because there is no E movement
   G1 X10 E20      ; move 10mm in X and E (assume absolute E positioning), a line is drawn
   G1 Y0 Y2
   M701 S1    ; only enable nozzle 1
   G1 X10 E30 ; move and eject only from nozzle 1
   

M702 xAABBBCCC......CCCBBBAAA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Buffer hex data string, to clock out during next E movements. 
Three nibbles make up one shot for the printhead (12 nozzles).
Data is added to the buffer. The buffer is read during the next E movement.

M703
~~~~
Reset the pixel write buffer pointer to the start. 


TODO:
~~~~~
Implement FIFO for pixel data, including a current status update (how much space is left in the FIFO)
to be able to write "endless" images. 



Peter Brier
~~~~~~~~~~~

