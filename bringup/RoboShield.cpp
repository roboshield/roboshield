#include "RoboShield.h"
#include "RoboShield_Defines.h"
#include <Arduino.h>


// Public methods
////////////////////////////////////////////////////////////////////////////////

bool RoboShield::buttonPressed(void) {
  return !digitalRead(BUTTON_PIN);
}

void RoboShield::setLED(bool on) {
  digitalWrite(LED_PIN, on);
}

float RoboShield::batteryVoltage(void) {
  return (float)(analogRead(BATTERY_VOLTAGE_PIN) * MAX_BATTERY_VOLTAGE / 1024.0);
}

void RoboShield::setPinMode(uint8_t pin, uint8_t mode) {
  if (pin < NUM_DIGITAL_PINS) {
    // this is a pin on the shield
    pinMode(DIGITAL_PIN_MAPPING[pin], mode);
  } else if (pin > MAX_USED_DIGITAL_PIN) {
    // this is a pin on the Arduino itself
    pinMode(pin, mode);
  } else {
    // this is an pin which is used internally by the shield
  }
}

bool RoboShield::readPin(uint8_t pin) {
  if (pin < NUM_DIGITAL_PINS) {
    // this is a pin on the shield
    return digitalRead(DIGITAL_PIN_MAPPING[pin]);
  } else if (pin > MAX_USED_DIGITAL_PIN) {
    // this is a pin on the Arduino itself
    return digitalRead(pin);
  } else {
    // this is an pin which is used internally by the shield
    return false;
  }
}

void RoboShield::setPin(uint8_t pin, bool set_high) {
  if (pin < NUM_DIGITAL_PINS) {
    // this is a pin on the shield
    digitalWrite(DIGITAL_PIN_MAPPING[pin], set_high);
  } else if (pin > MAX_USED_DIGITAL_PIN) {
    // this is a pin on the Arduino itself
    digitalWrite(pin, set_high);
  } else {
    // this is an pin which is used internally by the shield
  }
}

int RoboShield::getAnalog(uint8_t pin) {
  return analogRead(pin);
}

void RoboShield::setServos(uint8_t data) {
  setDataBus(data);
  digitalWrite(SERVO_LATCH_EN_PIN, HIGH);
  digitalWrite(SERVO_LATCH_EN_PIN, LOW);
}

void RoboShield::lcdSetCursor(uint8_t col, uint8_t row) {
  if (col >= 16 || row >= 2) {
    return;
  }
  const uint8_t addr = 0x80 + row * 0x40 + col;
  lcdControl(addr);
}

void RoboShield::lcdPrintChar(char c) {
  digitalWrite(LCD_RS_PIN, HIGH);
  setDataBus((uint8_t)c);
  digitalWrite(LCD_EN_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(LCD_EN_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_EN_PIN, LOW);
  delayMicroseconds(100);
}


// Private methods
////////////////////////////////////////////////////////////////////////////////

void RoboShield::init(void) {
  // only run this once
  static bool initialized = false;
  if (initialized) {
    return;
  }
  initialized = true;

  // configure any pins we don't want to be INPUT (the default)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PWN0_PIN, OUTPUT);
  pinMode(PWN1_PIN, OUTPUT);
  pinMode(PWN2_PIN, OUTPUT);
  pinMode(PWN3_PIN, OUTPUT);
  pinMode(SHIFT_REG_CLK_PIN, OUTPUT);
  digitalWrite(SHIFT_REG_CLK_PIN, LOW);
  pinMode(SHIFT_REG_DATA_PIN, OUTPUT);
  pinMode(SERVO_LATCH_EN_PIN, OUTPUT);
  pinMode(MOTOR_LATCH_EN_PIN, OUTPUT);
  pinMode(LCD_RS_PIN, OUTPUT);
  pinMode(LCD_EN_PIN, OUTPUT);
  
  // initialize the LCD
  // NOTE: Using delay() here doesn't work for some reason, so we use delayMicroseconds instead
  delayMicroseconds(15000);
  digitalWrite(LCD_EN_PIN, LOW);
  digitalWrite(LCD_RS_PIN, LOW);
  // send function set command sequence
  lcdControl(0x38);
  delayMicroseconds(4500);
  // second try
  lcdControl(0x38);
  delayMicroseconds(150);
  // set # lines, font size, etc
  lcdControl(0x38);
  // turn display on with no cursor or blinking default
  lcdControl(0x0C);
  // clear the display
  lcdControl(0x01);
  delayMicroseconds(3300);
  // set entry mode
  lcdControl(0x06);
}

void RoboShield::setDataBus(uint8_t data) {
  // set the value of the data bus
  uint8_t i;
  for (i = 0; i < 8; ++i) {
    digitalWrite(SHIFT_REG_DATA_PIN, (data & (1 << (7 - i))) ? HIGH : LOW);
    delayMicroseconds(1);
    digitalWrite(SHIFT_REG_CLK_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(SHIFT_REG_CLK_PIN, LOW);
    delayMicroseconds(1);
  }
}

void RoboShield::lcdControl(uint8_t data) {
  digitalWrite(LCD_RS_PIN, LOW);
  setDataBus(data);
  digitalWrite(LCD_EN_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(LCD_EN_PIN, HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_EN_PIN, LOW);
  delayMicroseconds(100);
}