
# RC Rover

![Rover on Vulcano](img/rover_on_vulcano.png)


This project was developed as part of a high school competition organized by the German Aerospace Center (DLR). The challenge: design and build a rover capable of collecting sensor data, navigating autonomously, and using a robotic arm to gather rock samples.

To enable wireless control from any BLE-capable device, I developed custom microcontroller software, a dedicated web app, and designed a bespoke PCB (unfortunately, the PCB was never manufactured due to supply issues). The foundation for the code was a project from my teacher, which I significantly modified and expanded.

While I focused on the electronics and software, my teammates handled the mechanical engineering. If you’re interested in recreating this project, please refer to the acknowledgements section for details on their contributions and additional resources.

## Project Screenshots:
![Webinterface0](img/webinterface0.png)
![Webinterface0](img/webinterface1.png)
![Webinterface0](img/webinterface2.png)
![PCB](img/pcb.png)
![perfboard](img/perfboard.png)
![Rover](img/rover.png)


## Installation
### Flash mcu-sw onto the ESP32
#### 1. Install Prerequisites
You’ll need VS Code or CLion with the PlatformIO extension.
See the Links section below for tutorials.

#### 2. Download the Project
Clone or download this repository and open it in your chosen IDE.

#### 3. Generate New UUIDs
Open the ESP32ble.h file. Go to uuidgenerator.net and generate new UUIDs. Copy and save these UUIDs for later use.

#### 4. Flash the ESP32
Upload (flash) the code to your ESP32 device.

#### 5. Test BLE Communication
Use an app like nRF Connect to check if your sensor is sending data via BLE.


## Acknowledgements

 - [RC Car Project by my Teacher](https://github.com/bkZuendorf/rc-car)
 - [3D-Printable Parts](https://github.com/matiassingers/awesome-readme)
 


## Authors

- [@TheFlocke](https://github.com/TheFlocke)
- [@xNiicki](https://github.com/xNiicki)


