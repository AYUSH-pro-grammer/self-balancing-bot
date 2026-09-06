# Self-Balancing Robot 🤖

Custom-built two-wheeled self-balancing robot with **ESP32**, **MPU-9250 IMU**, and **two BTS7960/IBT-2 motor drivers**.

![Robot Balancing](image/balancing.png)

## What is it?

It is a two-wheeled robot with an IMU which detects body tilt and drives motors to balance itself.

The ESP32 gets the information from the IMU about its acceleration and rotation speed, determines its tilt angle by using complementary filter and then balances its position by controlling two DC motors.

The self-balancing robot is similar to an inverted pendulum:

```text
Robot tilts → IMU detects changes → ESP32 determines angle
→ PID determines correction → Motor drives → Robot balances
```

![Robot Standing](image/standing.png)

## Why did I build it?

I wanted to create a project which involves mechanical construction, electronics, sensors, motor driving, and programming.

Self-balancing robots are particularly interesting projects since both mechanical and software part directly influence on the result.

Through this project, I gained experience in:

* ESP32 programming
* IMU sensors usage
* I²C communication
* Accelerometer and gyroscope calibration
* Complementary filter usage
* PID control
* Motor drivers programming
* CAD and mechanical design


## Hardware

| Component                    |   Qty |
| ---------------------------- | ----: |
| ESP32 DOIT DevKit V1         |     1 |
| GY-9250 / MPU-9250 IMU       |     1 |
| BTS7960 / IBT-2 Motor Driver |     2 |
| 12V DC Gear Motor            |     2 |
| Wheels                       |     2 |
| 12V Battery                  |     1 |
| 12V → 5V Buck Converter      |     1 |
| Custom chassis               |     1 |
| Wiring and connectors        | 1 set |

## Wiring

![Circuit Diagram](image/circuit.png)

### MPU-9250 → ESP32

| MPU-9250 | ESP32   |
| -------- | ------- |
| VCC      | 3.3V    |
| GND      | GND     |
| SDA      | GPIO 21 |
| SCL      | GPIO 22 |

The MPU communicates using I²C at address `0x68`.

### Motor Driver #1 → ESP32

| IBT-2 Pin | ESP32   |
| --------- | ------- |
| RPWM      | GPIO 25 |
| LPWM      | GPIO 26 |
| R_EN      | GPIO 27 |
| L_EN      | GPIO 13 |
| GND       | GND     |
| VCC       | 5V      |

Motor 1 connects to `M+` and `M-`.

### Motor Driver #2 → ESP32

| IBT-2 Pin | ESP32   |
| --------- | ------- |
| RPWM      | GPIO 32 |
| LPWM      | GPIO 33 |
| R_EN      | GPIO 14 |
| L_EN      | GPIO 19 |
| GND       | GND     |
| VCC       | 5V      |

Motor 2 connects to `M+` and `M-`.

## Power

The 12V battery powers both motor drivers directly:

```text
Battery + → IBT-2 #1 B+
          → IBT-2 #2 B+
          → Buck IN+

Battery - → IBT-2 #1 B-
          → IBT-2 #2 B-
          → Buck IN-
```

The buck converter provides 5V:

```text
Buck 5V → ESP32 5V/VIN
        → IBT-2 #1 VCC
        → IBT-2 #2 VCC
```

All grounds must be connected together:

```text
Battery - + Buck GND + ESP32 GND + both IBT-2 GND
```


## Firmware

The firmware is located at:

```text
firmware/main.cpp
```

It:

* Reads the MPU-9250 via I²C
* Calibrates the gyroscope
* Computes the tilt angle of the robot
* Applies a complementary filter
* Implements a PID control algorithm
* Controls both motors
* Halt the motors when the robot deviates beyond certain threshold from its target angle

The code is compiled for:

```text
DOIT ESP32 DEVKIT V1
```

Launch the serial monitor at **115200 baud**.

When starting, hold the robot steady while the IMU is calibrating.


## PID

The robot checks its current angle against a target angle and then computes a value to correct it by using PID control:

```text
PID = (Kp × Error) + (Ki × Integral) + (Kd × Derivative)
```

Actual values are:

```cpp
targetAngle = 3.46;
Kp = 10.0;
Ki = 0.0;
Kd = 0.5;
```

These values might be adjusted according to the future robot assembly.

## CAD

CAD files are present in:

```text
cad/self balancing.f3z
cad/self balancing.step
```

The CAD assembly consists of the design of the robot.

## Videos

### Working

[`video/working/video-w-1.mp4`](video/working/video-w-1.mp4)

### Testing


* [`video/testing/video-f-1.mp4`](video/testing/video-f-1.mp4)
* [`video/testing/video-f-2.mp4`](video/testing/video-f-2.mp4)


## Bill of Materials

A detailed BOM is available in:

[`bom.csv`](bom.csv)

| Item | Qty | Purchase Link |
| :--- | :---: | :--- |
| ESP32 DOIT DevKit V1 | 1 | [Robu.in Link](https://robu.in/product/esp-wroom-32-esp32-wifi-bt-ble-mcu-module/) |
| GY-9250 / MPU-9250 | 1 | [Robokits Link](https://robokits.co.in/sensors/gyroscope-and-inertial-imu/9dof-3-axis-accelerometer-gyroscope-magnetometer-gy-9250?srsltid=AfmBOopiH0Up3o8P3-oHTvJcObh-OI1lNOijdAXyQr8ByRqUDNTo8jB7DdQ) |
| BTS7960 / IBT-2 | 2 | [Robu.in Link](https://robu.in/product/double-bts7960-43a-h-bridge-high-power-stepper-motor-driver-module/) |
| 12V DC Gear Motor | 2 | [Robocraze Link](https://robocraze.com/products/200-rpm-geared-motor?variant=40192492601497&country=IN&currency=INR&utm_medium=product_sync&utm_source=google&utm_content=sag_organic&utm_campaign=sag_organic&srsltid=AfmBOopKAbyO-XIRKLJx-OmosgXKJial2h5KzaEtUzNf0ObJwdZQ6by-A0U) |
| Wheels | 2 | [Robomart Link](https://robomart.com/product/5x2-wheel-robotic-tyre-for-robotics-diy-for-dc-gear-motor-5x2-cm/?srsltid=AfmBOoqEX9lZ8_wr8UwCeNvzkUKPLWCdvC01yqPme6XPvA1c9YBfUza4lSY) |
| 12V Battery | 1 | [Electronicspices Link](https://electronicspices.com/product/18650-2600mah-3-65v-cylindrical-lithium-ion-cell?srsltid=AfmBOorxf52YEWnK8CLL8YlFW5W6aVtw5Zjrw4-55JQey9dv0Gju4ACj2Q8) |
| 12V → 5V Buck Converter | 1 | [RajivElectronics Link](https://rajivelectronics.com/product/lm2596-dc-dc-buck-converter-adjustable-step-down-module?srsltid=AfmBOoryoWfYvhn5YVpWpyPrIyInsibDxFsjgWKL2nmv9f-ZqJS0QHG6Jwc) |
| 3D printed chassis | 1 | - |


## Repository Structure

```text
.
├── README.md
├── bom.csv
├── cad
│   ├── self balancing.f3z
│   └── self balancing.step
├── firmware
│   └── main.cpp
├── image
│   ├── balancing.png
│   ├── circuit.png
│   └── standing.png
└── video
    ├── testing
    │   ├── video-f-1.mp4
    │   └── video-f-2.mp4
    └── working
        └── video-w-1.mp4
```


