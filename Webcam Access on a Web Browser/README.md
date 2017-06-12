# Webcam Access on a Web Browser

Based on the DragonBoard™ 410c development board from Arrow Electronics, a demo for image acquisition and web release was developed using the Python language. It is designed to demonstrate how to acquire images by camera and upload them to a web server deployed on DragonBoard 410c development board. Such images should then be accessible by other devices through network.

## Objective

This project is a simplified example of photo shooting and uploading, and can be conveniently integrated to a developer’s projects in order to realize real-time image acquisition and web release.

## Build / Assembly Instructions

**Materials Required / Parts List / Tools**

- DragonBoard 410c development board
- 12v wall adapter
- USB camera

**Build / Assembly**

- Install the Linux operating system on DragonBoard 410c
- Install boa web server by apt-get install boa command
- Copy the code to var/WWW directory
- Start up the boa server

**Usage Instructions**
After the server is started, connect the DragonBoard 410c into the network, open the notebook browser connected to the same router, enter the board IP, and you will access the USB image information.
