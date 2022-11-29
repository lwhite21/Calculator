#include <CowPi.h>
#include <cowpi_atmega328p.h>
#include <cowpi_boards.h>
#include <cowpi_dummy_pgmspace.h>
#include <cowpi_font_7segment.h>
#include <cowpi_font_dotmatrix.h>
#include <cowpi_fonts.h>
#include <cowpi_internal.h>
#include <cowpi_io.h>
#include <cowpi_lcd1602.h>
#include <cowpi_max7219.h>
#include <cowpi_setup.h>

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
 volatile cowpi_timer16bit_t *timer;
// volatile cowpi_timer8bit_t *timer;
volatile uint8_t *timer_interrupt_masks;

void setup() {
  cowpi_stdio_setup(9600);
  cowpi_set_display_i2c_address(0x27);
  cowpi_setup(LCD1602 | I2C);
  cowpi_lcd1602_set_backlight(true);
volatile cowpi_timer16bit_t *timer = (cowpi_timer16bit_t *)( cowpi_io_base + 0x60); 
  timer->control = 0b0000001100000100;
  timer->compareA = 249;
  // timer->interrupt_frequency = 16;
  // prescaler = 64;
  // frequency = 16;  
  // time_between_counts = 64;
   TCCR1A = 0;  
   TCNT1 = 0;
   TCCR1B  = 0;
  // TIMSK1 |= 0b00100001;
  timer_interrupt_masks = cowpi_io_base + 0x4E;
  timer_interrupt_masks[0] |= 0x2;
  attachInterrupt(digitalPinToInterrupt(2), handle_buttonpress, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), handle_keypress, CHANGE);
}
// ISR(TIMER1_COMPA_vect){
//   i++;
//   if(i==1){
//   C1 = ICR1;
//   } 
//   if(i ==2){
//     C2 = ICR1;
//     difference = C2-C1;
//     C1= 0;
//     C2 = 0;
//     i = 0;
//   }
// }
ISR(TIMER1_COMPA_vect){
  // capNum = 0;
  // capNum++;
  // if(capNum ==1){
  //   A1 = ICR1;
  //   TCNT1 = overflowCount;
  // }
  // if(capNum == 2){
  //   A2 = ICR1;
  //   TCNT1 = overflowCount;
    // capture = true;
  // }
// capState =  !capState;
// digitalWrite(5, capState);
}
void loop() {

  // while(A1 !=0 || A2 != 0){
  // TCNT1 = 249 - A1 + (ovfCountC2-ovfCountC1 -1) * 65536 + A2;
  //   Time = float(timer1Counts)*64.0e-6;

  //   display.clearDisplay();
  //   display.print("TIME");
  //   display.print(Time);
  //   delay(200);

  
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