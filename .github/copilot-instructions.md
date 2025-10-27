# RC Rover Development Instructions

ALWAYS follow these instructions first. Only use additional search and context gathering if the information here is incomplete or found to be in error.

## Project Overview
RC Rover is a Bluetooth Low Energy (BLE) controlled rover project with two main components:
- **WebApp**: HTML5/CSS/JavaScript web controller that connects via Web Bluetooth API
- **mcu-sw**: ESP32-S3 microcontroller firmware in C++ using PlatformIO framework

## Working Effectively

### Initial Setup and Dependencies
```bash
# Install PlatformIO for ESP32 development
pip3 install platformio
export PATH=$PATH:~/.local/bin
```

### WebApp Development
- Run the web application:
  ```bash
  cd WebApp
  python3 -m http.server 8080
  # Access at http://localhost:8080
  ```
- **VALIDATION**: Takes < 5 seconds to start. Test all 3 pages: Drivingcontrol, Armcontrol, Settings.
- The WebApp is a static web application - no build process required.
- **Known Issue**: Service worker registration fails in some environments but app still functions correctly.
- **Known Issue**: FontAwesome CDN may be blocked but icons still render properly.

### MCU Software Development  
- **CRITICAL**: ESP32 platform installation requires internet access and takes 3-5 minutes. NEVER CANCEL.
- **CRITICAL**: Build process takes 5-8 minutes for clean builds. NEVER CANCEL. Set timeout to 15+ minutes.

```bash
cd mcu-sw
export PATH=$PATH:~/.local/bin

# Install ESP32 platform (NEVER CANCEL - takes 3-5 minutes)
pio pkg install --platform espressif32

# Build firmware (NEVER CANCEL - takes 5-8 minutes) 
pio run

# Upload to ESP32-S3 (requires physical device)
pio run --target upload

# Monitor serial output
pio device monitor --baud 115200
```

### Build Time Expectations
- **WebApp startup**: < 5 seconds
- **PlatformIO installation**: ~90 seconds  
- **ESP32 platform download**: 3-5 minutes - NEVER CANCEL
- **Clean firmware build**: 5-8 minutes - NEVER CANCEL
- **Incremental builds**: 30-90 seconds
- **Upload to device**: 10-30 seconds

## Validation Scenarios

### WebApp Testing
ALWAYS test the following complete scenarios after making changes:
1. **Navigation Test**: Click all 3 page buttons (Drivingcontrol, Armcontrol, Settings) and verify page switches
2. **Control Interface Test**: Move sliders on driving page and verify values change
3. **Arm Control Test**: Verify arm control circles are visible and interactive
4. **Settings Test**: Verify min/max input fields accept values for wheels and arms
5. **BLE Interface Test**: Verify connection status shows "not connected" and BLE error message appears

### MCU Software Testing  
ALWAYS test these scenarios when hardware is available:
1. **Build Verification**: Ensure firmware compiles without errors
2. **Upload Test**: Flash firmware to ESP32-S3 and verify no upload errors
3. **BLE Advertisement**: Use nRF Connect app to verify "rc-rover" device appears
4. **Sensor Reading**: Connect BME680 sensor and verify environmental data
5. **Motor Control**: Connect servos/steppers and test movement commands

## Code Navigation

### WebApp Structure
```
WebApp/
├── index.html          # Main HTML interface with 3 pages
├── manifest.json       # PWA configuration  
├── src/
│   ├── js/
│   │   ├── main.js     # Core BLE communication and control logic
│   │   ├── window.js   # Arm control and UI interactions  
│   │   ├── sensor.js   # Sensor data display
│   │   └── debug.js    # Error handling and logging
│   └── styles.css      # All styling and responsive design
└── icons/              # PWA icons and favicons
```

### MCU Software Structure
```
mcu-sw/
├── platformio.ini      # PlatformIO configuration and dependencies
├── src/
│   ├── main.cpp        # Main program loop and task creation
│   ├── ESP32ble.cpp    # BLE server and communication handling
│   ├── Servo.cpp       # PWM servo control via I2C
│   ├── Stepper.cpp     # Stepper motor control 
│   ├── Sensor.cpp      # BME680 environmental sensor reading
│   └── I2C.cpp         # I2C bus management with mutex
└── lib/
    ├── ESP32ble.h      # BLE service and characteristic definitions
    ├── Servo.h         # Servo control interface
    ├── Stepper.h       # Stepper motor interface
    ├── Sensor.h        # BME680 sensor interface
    └── I2C.h           # I2C communication interface
```

## Key Dependencies and Libraries

### WebApp
- **Web Bluetooth API**: For BLE communication (requires HTTPS or localhost)
- **FontAwesome**: For UI icons (loaded from CDN)
- **No build dependencies**: Pure HTML/CSS/JavaScript

### MCU Software  
- **NimBLE-Arduino** (^2.2.3): Bluetooth Low Energy stack
- **Adafruit PWM Servo Driver Library** (^2.4.0): I2C servo control
- **FastAccelStepper** (^0.31.6): Stepper motor control
- **bsec2** (^1.10.2610): Bosch sensor library
- **BME68x Sensor library** (^1.3.40408): Environmental sensor

## Common Issues and Solutions

### WebApp Issues
- **Web Bluetooth not available**: Use Chrome/Edge on HTTPS or localhost
- **Service worker fails**: App still functions - ignore error in development  
- **FontAwesome blocked**: Icons still render - cosmetic issue only

### MCU Software Issues
- **Platform install fails**: Check internet connection - required for ESP32 platform download
- **Build fails with library errors**: Run `pio pkg install` to download dependencies
- **Upload fails**: Check ESP32-S3 is connected and in bootloader mode
- **BLE not advertising**: Verify UUIDs are unique - regenerate at uuidgenerator.net

## Development Workflow

### Making WebApp Changes
1. Edit HTML/CSS/JavaScript files directly
2. Refresh browser to see changes immediately  
3. Test all 3 pages and BLE connection interface
4. Verify responsive design on different screen sizes

### Making MCU Changes
1. Modify C++ source files in `src/` or headers in `lib/`
2. Build with `pio run` (NEVER CANCEL - wait 5-8 minutes)
3. Upload to device with `pio run --target upload`
4. Monitor serial output with `pio device monitor --baud 115200`
5. Test BLE connectivity with WebApp or nRF Connect

## BLE Protocol
- **Service UUID**: 5eaf1079-e806-47a9-a1ec-d815bea94805 (commands)
- **Service UUID**: 8da7a992-e263-4b78-abf2-bdb94808895c (sensors)
- **Device Name**: "rc-rover"
- **Command Format**: "id:value" (e.g., "0:127" for motor speed)

ALWAYS regenerate UUIDs when creating a new rover instance to avoid BLE conflicts.

## Hardware Requirements
- **ESP32-S3 DevKitC-1**: Main microcontroller board
- **BME680**: Environmental sensor (temperature, humidity, pressure, gas)
- **PCA9685**: I2C PWM driver for servos  
- **TMC2209**: Stepper motor drivers
- **Servos**: For robotic arm control (4 servos)
- **Stepper Motors**: For wheel drive (2 motors)

## Safety Notes
- Servo angles are constrained to 0-180° in firmware
- Stepper speeds have safe limits in code
- I2C communications use mutex for thread safety
- BLE disconnection automatically stops all motors

## Common Tasks

### Repository Root Structure
```
ls -la
.
..
.DS_Store
.git
.gitignore
.idea/
LICENSE
PCB/                    # PCB design files (KiCad)
README.md
WebApp/                 # Web application
img/                    # Project screenshots and documentation images
mcu-sw/                 # ESP32-S3 firmware
```

### WebApp Directory Contents
```
ls -la WebApp/
.gitignore
icons/                  # PWA icons and favicons
img/                    # Image assets for UI
index.html             # Main application entry point
manifest.json          # Progressive Web App manifest
src/                   # Source code (JavaScript and CSS)
twa-manifest.json      # Trusted Web Activity manifest
```

### MCU Software Directory Contents  
```
ls -la mcu-sw/
.gitignore
include/               # PlatformIO include directory
lib/                   # Custom library headers
platformio.ini         # PlatformIO project configuration
src/                   # C++ source files
test/                  # Unit test directory (currently empty)
```

### Expected Build Outputs
After successful PlatformIO build in `mcu-sw/.pio/`:
- `build/esp32-s3-devkitc-1/firmware.bin` - Main firmware binary
- `build/esp32-s3-devkitc-1/firmware.elf` - ELF executable with debug symbols
- `libdeps/` - Downloaded library dependencies
- PlatformIO automatically manages build artifacts - no manual cleanup needed