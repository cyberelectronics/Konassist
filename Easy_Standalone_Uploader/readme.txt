Should work with any ESP32 board, just be sure GPIO35 pin is HIGH. 
(you can pullup GPIO35 to 3V3, with an external 10K resistor and add a Push Button, between GPIO35 and GND).
Otherwise ESP32 can enter and remain in AP mode, after each restart if GPIO35 is LOW (0V).