/*
  ============================================================
  Analog Joystick Tester for Arduino
  ============================================================
  Display: LCD 16x2 via I2C adapter (address is usually 0x27 or 0x3F)
  Library: "LiquidCrystal I2C" (by Frank de Brabander) — 
           install via Library Manager in Arduino IDE.

  Wiring (standard KY-023 type module):
    VRx (X)        -> A0
    VRy (Y)        -> A1
    SW  (button)   -> D2   (if the module has no button — set HAS_BUTTON = false)
    +5V            -> 5V
    GND            -> GND

  LCD I2C (on Nano):
    SDA -> A4
    SCL -> A5
    VCC -> 5V
    GND -> GND

  HOW TO USE:
    1) Find the ADC range of your specific joystick model in the datasheet 
       or online (usually 0-1023, but center and edges vary by board).
       If unknown, set SHOW_RAW_VALUES = true, upload the code, 
       move the joystick in all directions, and note the min/center/max 
       values displayed on the screen (or Serial Monitor).
    2) Enter those values into X_MIN/X_CENTER/X_MAX and 
       Y_MIN/Y_CENTER/Y_MAX.
    3) Set SHOW_RAW_VALUES = false — the screen will show 
       normalized values (range OUT_MIN..OUT_MAX) taking into account 
       calibration, axis inversion, and the deadzone.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================================================================
//                 SETTINGS FOR YOUR JOYSTICK MODEL
// ================================================================

// ---- Pin connections ----
const uint8_t PIN_X = A0;
const uint8_t PIN_Y = A1;
const uint8_t PIN_BUTTON = 2;

// ---- Does the joystick have a button? ----
const bool HAS_BUTTON = true;

// ---- Button logic:
//      true  = pulls to GND when pressed (requires INPUT_PULLUP,
//              standard for most modules)
//      false = outputs HIGH when pressed ----
const bool BUTTON_ACTIVE_LOW = true;

// ---- Calibration of the specific model's ADC (see instructions above) ----
int X_MIN = 0;
int X_CENTER = 512;
int X_MAX = 1023;

int Y_MIN = 0;
int Y_CENTER = 512;
int Y_MAX = 1023;

// ---- Axis inversion (some modules are physically rotated) ----
const bool INVERT_X = false;
const bool INVERT_Y = false;

// ---- Range of output values on the screen in processed mode ----
const int OUT_MIN = -100;
const int OUT_MAX = 100;

// ---- Center deadzone, in ADC units (to prevent potentiometer drift) ----
const int DEADZONE = 15;

// ---- Screen mode:
//      true  — raw ADC values (useful for calibration)
//      false — processed values (calibrated/inverted/deadzone applied) ----
const bool SHOW_RAW_VALUES = true;

// ---- LCD I2C address (usually 0x27 or 0x3F) ----
const uint8_t LCD_ADDRESS = 0x27;

// ---- Duplicate output to Serial Monitor (9600 baud) ----
const bool ENABLE_SERIAL_DEBUG = true;

// ---- Screen update interval, ms ----
const unsigned long UPDATE_INTERVAL = 120;

// ================================================================
//                          END OF SETTINGS
// ================================================================

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);
unsigned long lastUpdate = 0;

int processAxis(int raw, int minV, int centerV, int maxV, bool invert) {
  int value;
  if (raw <= centerV) {
    value = map(raw, minV, centerV, OUT_MIN, 0);
  } else {
    value = map(raw, centerV, maxV, 0, OUT_MAX);
  }
  value = constrain(value, OUT_MIN, OUT_MAX);
  return invert ? -value : value;
}

void setup() {
  if (ENABLE_SERIAL_DEBUG) Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  if (HAS_BUTTON) {
    pinMode(PIN_BUTTON, BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT);
  }

  lcd.setCursor(0, 0);
  lcd.print("Joystick Test");
  delay(1000);
  lcd.clear();
}

void loop() {
  if (millis() - lastUpdate < UPDATE_INTERVAL) return;
  lastUpdate = millis();

  int rawX = analogRead(PIN_X);
  int rawY = analogRead(PIN_Y);

  bool pressed = false;
  if (HAS_BUTTON) {
    int state = digitalRead(PIN_BUTTON);
    pressed = BUTTON_ACTIVE_LOW ? (state == LOW) : (state == HIGH);
  }

  int xOut, yOut;
  if (SHOW_RAW_VALUES) {
    xOut = rawX;
    yOut = rawY;
  } else {
    int adjX = (abs(rawX - X_CENTER) < DEADZONE) ? X_CENTER : rawX;
    int adjY = (abs(rawY - Y_CENTER) < DEADZONE) ? Y_CENTER : rawY;
    xOut = processAxis(adjX, X_MIN, X_CENTER, X_MAX, INVERT_X);
    yOut = processAxis(adjY, Y_MIN, Y_CENTER, Y_MAX, INVERT_Y);
  }

  char valStr[8];
  char line[17];
  
  // Top row — X
  itoa(xOut, valStr, 10);
  snprintf(line, sizeof(line), "X:%-14s", valStr);
  lcd.setCursor(0, 0);
  lcd.print(line);
  
  // Bottom row — Y + button indicator on the right
  itoa(yOut, valStr, 10);
  snprintf(line, sizeof(line), "Y:%-10s%4s", valStr, pressed ? "BTN" : "");
  lcd.setCursor(0, 1);
  lcd.print(line);

  if (ENABLE_SERIAL_DEBUG) {
    Serial.print("X_raw="); Serial.print(rawX);
    Serial.print(" Y_raw="); Serial.print(rawY);
    Serial.print(" X_out="); Serial.print(xOut);
    Serial.print(" Y_out="); Serial.print(yOut);
    Serial.print(" BTN="); Serial.println(pressed ? "1" : "0");
  }
}
