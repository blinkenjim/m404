M404 Monitor

This project is based on the Futaba M404SD01, a vacuum fluorescent display (VFD)
module with a 40-character by 4-line display. The M404 Monitor is intended to
display status - date, time, IP addresses, and system load - for a headless
Raspberry Pi computer. The project has three major parts: a 3D-printed case
with laser-cut acrylic front and back covers; an Arduino Nano that drives the
VFD module; and a python script that runs as a system service on the Raspberry
Pi. The Python script gathers information on the Raspberry Pi, sends it to the
Arduino, which forwards the information to the Futaba VFD module.

The parts of this project are:

* 3d - The Autodesk Fusion project file, and exported STL and SVG files.
* Aurduino - The code that runs on the Arduino Nano.
* Python - The python script and shell script that constitute the system service
           for the Raspberry Pi.
* Ref - a copy of the Futaba manual for the M404SD01 VFD module.
