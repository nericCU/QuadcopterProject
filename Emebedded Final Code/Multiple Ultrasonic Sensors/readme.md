Built and tested to run on the Arduino Pro Mini

This program utilzies three UltraSonic Sensors on pins   10,11   A0,A1   and   A2,A3 (trigger followed by echo)
Corresponding to the Left, Front, and Right sensor.

There is a delay of 30ms between pings. It pings it in the same order as above.

If the distance is within our range ( 1 cm < x < 50 cm) then it writes to the serial port.


