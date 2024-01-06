# Home-Automation---Lighting Project

## Overview
This project aims to allow a TV Remote Control to turn light switches on and off. It consists of two main modules: an infrared signal sender module and a receiver module. 

## Sender Module
The sender module receives infrared signals from a TV Remote Control and communicates with the receiver module through Bluetooth Low Energy (BLE). 

## Receiver Module
The receiver module receives information from the sender module through Bluetooth Low Energy (BLE) and controls two servo motors to turn light switch on and off. 

## Next steps and updates
Currently, this program was edited on Arduino IDE and BLE functionality is based off of an example program. I hope to recreate the same thing on Platform IO with a dedicated or imported library that can be used on not only ESP32, but any microcontroller with the BLE functionality. 

## Disclaimer
The code that is uploaded here is mostly uses example code provided through the creator of the library at use on the Arduino IDE. Portion of it has been modified such as
usage of servo motors, program flow, usage of infrared receivers, and the usage of hardware interrupts. 
