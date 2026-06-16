# Arduino Analog Joystick Tester
This project is a simple yet effective tool for testing analog joysticks (like the common KY-023 module) using an Arduino Nano and an I2C 16x2 LCD display. It helps you verify analog input values, calibrate the center and range of your potentiometers, and test the integrated push button.
## Features
* **Real-time Monitoring:** View X and Y axis coordinates directly on an LCD screen.
* **Calibration Mode:** Use the `SHOW_RAW_VALUES` setting to identify the exact ADC range and center point of your specific hardware.
* **Normalized Mode:** Easily map your joystick movement to a custom range (e.g., -100 to 100) with configurable deadzones to eliminate potentiometer drift.
* **Serial Debugging:** Optionally output all data to the Serial Monitor for easier logging.
## Wiring Diagram
| Joystick Pin | Arduino Pin |
| --- | --- |
| **VRx (X)** | A0 |
| **VRy (Y)** | A1 |
| **SW (Button)** | D2 |
| **+5V** | 5V |
| **GND** | GND |
For the LCD I2C display: connect SDA to A4 and SCL to A5 on the Arduino Nano.
## Setup Instructions
1. **Install Library:** Ensure you have the **LiquidCrystal I2C** library (by Frank de Brabander) installed via the Arduino IDE Library Manager.
2. **Calibration:**
* Set `SHOW_RAW_VALUES = true` in the code.
* Upload the sketch and move the joystick to find your `MIN`, `CENTER`, and `MAX` values for both axes.
* Update the variables `X_MIN`, `X_CENTER`, `X_MAX`, `Y_MIN`, `Y_CENTER`, and `Y_MAX` with your recorded values.
3. **Run:** Set `SHOW_RAW_VALUES = false` to enable the processed output mode.
## Configuration
All settings can be easily modified at the top of the `joystick_tester.ino` file:
* 
`DEADZONE`: Adjust this if your joystick values fluctuate while untouched.
* 
`INVERT_X` / `INVERT_Y`: Set to `true` if your axis output is moving in the wrong direction.
* 
`LCD_ADDRESS`: Set to `0x27` or `0x3F` depending on your specific I2C module.
*Developed for Arduino.*
