/**************************************************************************//**
 *
 * @file calculator.ino
 *
 * @author (STUDENTS -- Logan and Cailen)
 *
 * @brief Implementation of a 4-function integer calculator
 *
 ******************************************************************************/

/*
 * Calculator GroupLab assignment and starter code (c) 2021-22 Christopher A. Bohn
 * Calculator GroupLab solution (c) the above-named student
 */

#include "CowPi.h"

#define DEBOUNCE_TIME 20u


// UNCOMMENT THE STRUCTURE FOR THE TIMER YOU WLL USE
// volatile cowpi_timer16bit_t *timer;
// volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;

void setup() {
  cowpi_stdio_setup(9600);
  cowpi_set_display_i2c_address(0x27);
  cowpi_setup(LCD1602 | I2C);
  cowpi_lcd1602_set_backlight(true);
  // timer = ...;
  // timer->control = 0x32;
  // timer->compareA = 250-1;
  // timer_interrupt_masks = ...;
  timer_interrupt_masks[0] |= 0x2;
  attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);
}

void loop() {
  ;
}


void handle_buttonpress(void) {
  static unsigned long last_button_press = 0uL;
  static bool button_pressed = false;
  unsigned long now = millis();
  if (now - last_button_press > DEBOUNCE_TIME) {
    last_button_press = now;
    if (cowpi_left_button_is_pressed) {
      Serial.println("Left button was pressed");
    } else  {
      Serial.println("Right button was pressed");
    }
  }
}

void handle_keypress(void) {
  static unsigned long last_key_press = 0uL;
  static bool key_pressed = false;
  unsigned long now = millis();
  if (now - last_key_press > DEBOUNCE_TIME) {
    last_key_press = now;
    Serial.println(cowpi_get_keypress());
  }
}