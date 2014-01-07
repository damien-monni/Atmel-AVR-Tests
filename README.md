#Brushless motors init on a single Atmel AVR 16 bits timer

The aim of this code is to run multiple servos or brushless motors connected to ESC with a single 16 bits timers.
It creates a software PMW signal at 50Hz.

Everything is done with interrupt so there is no code at all in the never ending loop `while(1)`. This allow it to be edited so it can become a library.