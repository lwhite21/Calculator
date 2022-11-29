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
 volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;

void setup() {
  cowpi_stdio_setup(9600);
  cowpi_set_display_i2c_address(0x27);
  cowpi_setup(LCD1602 | I2C);
  cowpi_lcd1602_set_backlight(true);
  volatile cowpi_timer8bit_t *timer = (cowpi_timer8bit_t *)( cowpi_io_base + 0x90);
  //  timer->control = 0b0000010000000010;
  timer->control |= 0b00000100;
  timer->control |= 0b00000010;
  timer->compareA = 249;
   TCCR2A = 0;  
   TCNT2 = 0;
   TCCR2B  = 0;
   timer->counter = TCNT2;
  //  timer->attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, HIGH);
  //  timer->attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);
  timer_interrupt_masks = cowpi_io_base + 0x4E;
  timer_interrupt_masks[0] |= 0x2;
  attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);
  cowpi_lcd1602_place_character(0xF, '0');
}
ISR(TIMER2_COMPA_vect){
}
void loop() {
uint8_t time = millis();
uint8_t then = 0;
uint8_t now = 0;
uint8_t mytime = 0;
 then = now;
    Serial.println("timer has gone off");

now = TCNT2;
if (time - then >= 7500){
      Serial.println("timer has gone off");
}
else{ 
  mytime++;
}
// }if(time & 0x1D4C){
  // gpio[D8_D13].output |= 0x20;
  // Serial.println("timer has gone off");
}


void handle_buttonpress(void) {
  static unsigned long last_button_press = 0uL;
  static bool button_pressed = false;
  unsigned long now = millis();
  if (now - last_button_press > DEBOUNCE_TIME) {
    last_button_press = now;
    if (cowpi_left_button_is_pressed()) {
      negate_operator();
      cowpi_illuminate_left_led();
    } else if (cowpi_right_button_is_pressed()) {
      maunual_clear_display();
      cowpi_illuminate_right_led();
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
static int operator_1_negative = 1;
static int operator_2_negative = 1;
static int one_or_two = 1;
void display_keypress(char key) {
  if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4'
   || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
    int length = strlen(operator_2);
    if (length < 9) {
      operator_2[length] = key;
    }
  } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
    if (!(strlen(operator_1) > 0 && strlen(operator_2) <= 0)) {
      blank_display();
      cowpi_lcd1602_place_character(0x10, '0');
      operator_1_negative = operator_2_negative;
      operator_2_negative = 1;
      one_or_two = 2;
      memset(operator_1,0,sizeof(operator_1));
      strcpy(operator_1, operator_2);
    }
    one_or_two = 2;
    if (strlen(operator_1) == 0 && strlen(operator_2) <= 0) {
      operator_1[0] = '0';
    }
    memset(operator_2,0,sizeof(operator_2));
    if (key == 'A') {
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
    } 
  } else if (key == '#') {
    if (one_or_two == 1) {
      blank_display();
      strcpy(operator_1, operator_2);
      operator_1_negative = operator_2_negative;
      operator_2_negative = 1;
      memset(operator_2,0,sizeof(operator_2));
      
    } else {
      one_or_two = 1;
      long result;
      if (operation == 1) {
        result = ((long) strtol(operator_1, (char **)NULL, 10))*operator_1_negative + ((long) strtol(operator_2, (char **)NULL, 10))*operator_2_negative;
      } else if (operation == 2) {
        result = ((long) strtol(operator_1, (char **)NULL, 10))*operator_1_negative - ((long) strtol(operator_2, (char **)NULL, 10))*operator_2_negative;
      } else if (operation == 3) {
        result = ((long) strtol(operator_1, (char **)NULL, 10))*operator_1_negative * ((long) strtol(operator_2, (char **)NULL, 10))*operator_2_negative;
      } else if (operation == 4) {
        result = ((long) strtol(operator_1, (char **)NULL, 10))*operator_1_negative / ((long) strtol(operator_2, (char **)NULL, 10))*operator_2_negative;
      } 
      clear_display();
      if (result < 0) {
        result = result * -1;
        operator_1_negative = -1;
      } else {
        operator_1_negative = 1;
      }
      ltoa(result, operator_1, 10);
      operation = 0;
    }
  }

  display_operands();
}

void display_operands(void) {
  int i = strlen(operator_1);
  int j = strlen(operator_2);
  const char *c = operator_1;
  const char *s = operator_2;
  if (strlen(operator_1) > 0) {
    if (operator_1_negative == -1) {
      cowpi_lcd1602_place_character(0x10 - i - 1, '-');
    } else {
      cowpi_lcd1602_place_character(0x10 - i - 1, ' ');
    }
  }
  if (strlen(operator_2) > 0) {
    if (operator_2_negative == -1) {
      cowpi_lcd1602_place_character(0x50 - j - 1, '-');
    } else {
      cowpi_lcd1602_place_character(0x50 - j - 1, ' ');
    } 
  }
  while (*c != '\0') {
    cowpi_lcd1602_place_character(0x10 - i, *c++);
    i--;
  }
  while (*s != '\0') {
    cowpi_lcd1602_place_character(0x50 - j, *s++);
    j--;
  }
  if (strlen(operator_1) > 9) {
    clear_display();
    cowpi_lcd1602_place_character(0x10 - 1, 'r');
    cowpi_lcd1602_place_character(0x10 - 2, 'o');
    cowpi_lcd1602_place_character(0x10 - 3, 'r');
    cowpi_lcd1602_place_character(0x10 - 4, 'r');
    cowpi_lcd1602_place_character(0x10 - 5, 'E');
  }
}

void negate_operator() {
  if (one_or_two = 1) {
    operator_2_negative = operator_2_negative * -1;
  } else {
    operator_1_negative = operator_1_negative * -1;
  }
  display_operands();
}

void clear_display(void) {
  memset(operator_1,0,sizeof(operator_1));
  memset(operator_2,0,sizeof(operator_2));
  operator_1_negative = 1;
  operator_2_negative = 1;
  int index = 0;
  while (index < 16) {   
    cowpi_lcd1602_place_character(0x00 + index, ' ');
    cowpi_lcd1602_place_character(0x00 + index + 0x40 , ' ');
    index++;
  }
  cowpi_lcd1602_place_character(0xF, '0');
}

void maunual_clear_display(void) {
  if (one_or_two == 1) {
    clear_display();
  } else {
    memset(operator_2,0,sizeof(operator_2));
    operator_2_negative = 1;
    int index = 0;
    while (index < 16) {   
      cowpi_lcd1602_place_character(0x00 + index + 0x40 , ' ');
      index++;
    }
  }
}

void blank_display(void) {
  int index = 0;
  while (index < 16) {   
    cowpi_lcd1602_place_character(0x00 + index, ' ');
    cowpi_lcd1602_place_character(0x00 + index + 0x40 , ' ');
    index++;
  }
}