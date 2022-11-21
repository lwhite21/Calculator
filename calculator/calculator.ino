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
    if (cowpi_left_button_is_pressed()) {
      Serial.println("Left button was pressed");
    } else if (cowpi_right_button_is_pressed()) {
      Serial.println("Right button was pressed");
      clear_display();
    }
  }
}

void handle_keypress(void) {
  static unsigned long last_key_press = 0uL;
  static bool key_pressed = false;
  unsigned long now = millis();
  if (now - last_key_press > DEBOUNCE_TIME) {
    last_key_press = now;
    display_keypress(cowpi_get_keypress());
  }
}

static char operator_1[10] = "";
static char operator_2[10] = "";
static int operation = 0;
void display_keypress(char key) {
  if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4'
   || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
    if (cowpi_left_switch_in_left_position()) {
      int length = strlen(operator_1);
      if (length < 8) {
        operator_1[length] = key;
      }
    } else {
      int length = strlen(operator_2);
      if (length < 8) {
        operator_2[length] = key;
      }
    }
  } else if (key == 'A') {
    operation = 1;
    cowpi_lcd1602_place_character(0x40, '+');
  } else if (key == 'B') {
    operation = 2;
    cowpi_lcd1602_place_character(0x40, '-');
  } else if (key == 'C') {
    operation = 3;
    cowpi_lcd1602_place_character(0x40, 'x');
  } else if (key == 'D') {
    operation = 4;
    cowpi_lcd1602_place_character(0x40, 0xFD);
  } else if (key == '#') {
    if (operation == 1) {
      Serial.println(atoi(operator_1) + atoi(operator_2));
    } else if (operation == 2) {
      Serial.println(atoi(operator_1) - atoi(operator_2));
    } else if (operation == 3) {
      Serial.println(atoi(operator_1) * atoi(operator_2));
    } else if (operation == 4) {
      Serial.println(atoi(operator_1) / atoi(operator_2));
    } 
  }

  // display operands
  int i = strlen(operator_1);
  int j = strlen(operator_2);
  const char *c = operator_1;
  const char *s = operator_2;
  while (*c != '\0') {
    cowpi_lcd1602_place_character(0x10 - i, *c++);
    i--;
  }
  while (*s != '\0') {
    cowpi_lcd1602_place_character(0x50 - j, *s++);
    j--;
  }
}

void clear_display(void) {
  memset(operator_1,0,sizeof(operator_1));
  memset(operator_2,0,sizeof(operator_2));
  int index = 0;
  while (index < 16) {   
    cowpi_lcd1602_place_character(0x00 + index, ' ');
    cowpi_lcd1602_place_character(0x00 + index + 0x40 , ' ');
    index++;
  }
}