# ArduSBSkoot

A hoverboard hacking to build a wooden board self balancing scooter.

This hacking is made by interposing a PIC and an Arduino UNO inside the serial communication between one of the hoverboard's gyro cards and the motherboard.

The PIC reads the 9-bit serial communication from the gyro card and relays the data to Arduino at higher speed.
Arduino reads the data from the PIC and adjust it based on the control signal from a nunchuk or any other control different from the original 2-pedal control. Then data is sent to the hoverboard motherboard to control the wheels as originally.

The PIC is needed to allow parallel processing between the serial stream (PIC) and the control algorithm (Arduino).
