# Ultrasonic Fan Control
This project is designed to use the DragonBoard™ 410c from Arrow Electronics with a HS-100 ultrasonic module to detect the distance a person is from a fan to help prevent injury. The system is designed to transmit the distance value detected to the DragonBoard 410c and switch off the fan by triggering a relay connected to the DragonBoard 410c. The system is also designed to turn on the fan when the proximity near the fan is clear.

## Objective

Traditional fans with rotating blades can be hazardous to little children. News about children’s fingers being injured by fan blades is too frequent. This project uses the available resources to try to reduce such accidents.

## Build / Assembly Instructions

**Materials Required / Parts List / Tools**

- DragonBoard 410c development board
- 12v wall adapter
- Ultrasonic module 
- Relay 

**Build / Assembly**

- Install the Linux operating system on DragonBoard 410c
- Update source
- Install ultrasonic driver
- Run distance detection program

**Usage Instructions**

Power on to enter the system, install insmod sonar.ko, when lsmod indicates the driver is OK, run the script on the desktop to test its effects.
