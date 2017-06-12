#  Hand Gesture Recognition

The project is designed to implement a program that can recognize simple hand gestures of rock, paper and scissors on a DragonBoard™ 410c development board from Arrow Electronics that is hosting a Debian operating system, using Python development language and Open CV vision processing library.

## Objective

Aim of this demo is to introduce developers to the concept of image recognition and image processing on the DragonBoard 410c. Using a popular game of “Rock, Paper and Scissors”, developers can get familiar with the basic process of image collection, target image acquisition and recognition program design.

## Build / Assembly Instructions

**Materials Required / Parts List / Tools**
- DragonBoard 410c development board
- 12v wall adapter
- USB camera

**Build / Assembly**
- Install the Linux operating system on DragonBoard 410c
- Install the opencv-python library by apt-get install opencv-python command
- Copy codes to the user directory
- Run python shouxingShibie.py

**Usage Instructions**

After startup, put your hand on the red rectangular box region and the system will automatically detect hand gesture at intervals and output the hand gesture type, recognizing three hand gestures, namely rock, paper and scissors.

***
