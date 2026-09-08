# ESP32-WiFi-Fan-Controller
ESP32-based 12 V fan controller with Wi-Fi web interface, PWM speed control and RPM monitoring.

The system allows the user to control fan power and speed, set a one-minute timer and monitor fan RPM from a phone or computer connected to the same Wi-Fi network.

## Features

- Fan ON/OFF control
- PWM speed control from 0 to 100%
- RPM measurement using the TACH signal
- One-minute timer
- Web interface
- Local Wi-Fi control

## Hardware

- ESP32 development board
- 12 V 4-wire fan
- N-channel MOSFET
- 220 Ω gate resistor
- 10 kΩ pull-down resistor
- 12 V power supply

## Schematic

![Schematic](images/scheme ESP.png)

## Hardware Prototype

![Hardware Prototype](images/prototype.jpg)

## Web Interface

The interface allows the user to turn the fan ON/OFF, adjust PWM, start the one-minute timer and monitor RPM.

![Web Interface](images/web-interface.png)

## How It Works

The ESP32 connects to Wi-Fi and hosts a local HTTP server.

The browser communicates with the ESP32 using several endpoints:

- `/on` – turn the fan on
- `/off` – turn the fan off
- `/minute` – run the fan for one minute
- `/pwm?value=X` – set PWM from 0 to 100%
- `/rpm` – read current fan RPM

Fan speed is controlled using the PWM input. The TACH signal is counted using an interrupt and used to calculate RPM.

The one-minute timer uses `millis()`, so the ESP32 can continue handling web requests while the timer is running.

## Source Code

The firmware is written in C++ using the Arduino framework for ESP32.

The complete source code is available in [`src/main.cpp`](src/main.cpp).

#
**Working prototype**
