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
The code adopts a portion of the example code provided through the creator of the library on the Arduino IDE. Initialization/Establishment of BLE devices and BLE connections uses the example code however, it has been modified such as usage of servo motors, program flow, and usage of infrared receivers. I made sure to address which parts were adopted in my source code. 
