#LCD/Joystick Demo

This demo uses a 9 pin Apple II compatable joystick and displays its output on a Hitachi HD44780 compatible LCD.

Tested on PIC24FJ64GB004

##Pin out:
####LCD:
*RS: RB11
*E: RB10
*D4: RB9
*D5: RB8
*D6: RB7
*D7: RB5

####NOTE: 	
These pins were chosen because they are 5V tolerant. Using other pins with a 5V display may cause magic smoke.
 
####Joystick:
*X-Axis: RA0
*Y-Axis: RA1
*Button 1: RB14
*Button 2: RB15

####NOTE:	
1. Power the joystick with 3.3 V, otherwise you will toast the microcontroller.
2. X and Y axis pins must be connected to ground through 10 kOhm resistor (on these joysticks, the potentiometer is not internally connected to ground, adding this resistor creates a divider between the joystick resistance and ground)

For Apple II joysticks, you can find the pinout [here](http://www.1000bit.it/support/manuali/apple/R023PINOUTS.TXT) as the "Apple Game Port (9-pin connector)"

More information at [www.evenchick.com/projects/joystick.html](http://www.evenchick.com/projects/joystick.html)
