# Multiplexing an E-Bike Segment Display with Arduino
This project documents the process of reverse-engineering and multiplexing a custom E-bike segmented display using an Arduino micro-controller. The primary challenge was the lack of publicly available documentation for the display's pin-out and electrical characteristics.


## Components Used
1. The display I have used for this project can be found on [robu](https://robu.in/product/high-brightness-digital-screen-for-electric-scooter/#) (Part number: 201217).
2. Micro-controller: Arduino Uno (or compatible)
3. Resistors: A set of current-limiting resistors (220-330Ω recommended, adjust as needed)
Breadboard & Jumper Wires

## Display Specifications & Reverse Engineering
The display used in this project is a custom-built component, likely for a specific manufacturer. It can be identified by the part number 201217 printed on its side. Due to the complete absence of datasheets or public resources, the pinout and segment configurations were determined through a process of careful testing and reverse engineering.

**Note: The display is a common cathode type. To light a segment, the corresponding segment pin must be driven HIGH, and the common pin for the desired digit must be driven LOW.**

![](Display%20Resources/Segmented%20Display%20Pinout.webp)

## Segment Configurations
A key step in this project was mapping the pin combinations required to illuminate each individual segment. The patterns are documented in the following PDF. This serves as a critical lookup table for the Arduino code.

[View Segment Configuration PDF](Display%20Resources/201217%20E-Bike%20Segmented%20Display%20Configurations.pdf)

## Multiplexing Logic
The project's code utilizes a time-division multiplexing technique to control the display. By rapidly cycling through each digit, the illusion of a continuously lit display is created. The core logic involves:

1. Setting the segment pins for the current digit's value (e.g., 1, 2, 3).
2. Activating the corresponding common pin for that digit.
3. Pausing for a few milliseconds to allow the digit to be visible.
4. Deactivating the common pin and repeating the process for the next digit.

## Project Status
This is a test project to demonstrate the feasibility of controlling this specific display. Next steps may include implementing a more robust library for display control or integrating it with other e-bike components.


