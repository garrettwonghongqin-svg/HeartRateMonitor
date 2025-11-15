# Heart Rate Monitor
This project works on a heart rate monitor using an arduino, a pulse sensor, a breadboard and an LCD display screen.
The codes are done in Arduino Idle, using C programming language.

Description:
This Arduino project implements a real-time heart rate monitoring system using a pulse sensor, an interrupt-driven sampling algorithm, and a 20×4 LCD display. The code continuously measures signal peaks and troughs to detect heartbeats, calculates BPM using rolling averages for accuracy, and displays both the current heart rate and a simple status indicator (high, normal, or low) on the LCD. A built-in LED on pin 13 blinks with each detected pulse, and optional serial output allows the data to be viewed in the Arduino Serial Plotter or a Processing-based visualizer. Designed for reliability and clarity, the system uses timing interrupts (Timer2) to maintain consistent sampling, making it suitable for learning about biosignal processing, embedded systems, and real-time data acquisition.
