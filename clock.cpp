#include <LiquidCrystal.h>

// Arduino Pins
#define LCD_RS 2 
#define LCD_EN 3
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define BTN01  13
#define BTN02  12
#define BTN03  11
#define BTN04  10
#define BUZZER 9
#define LED    8

// Buzzer Notes
#define NOTE_A4  440

// Possible FSM States
typedef enum {
  NORMAL,
  ALARM_TRIGGERED,
  SELECT_FORMAT,
  ADJUST_HOUR,
  ADJUST_MINUTE,
  ADJUST_SECOND,
  ALARM_HOUR,
  ALARM_MINUTE,
  ALARM_SECOND
} states;

// Create LCD handle
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Clock Variables
int clock_seconds = 0;
int clock_minutes = 0;
int clock_hours = 20;
bool mode_24h = true;
int alarm_seconds = 0;
int alarm_minutes = 0;
int alarm_hours = 0;
bool alarm_enabled = false;

// Control Variables
states current_state = NORMAL;
int led_level = LOW;
int ms_elapsed = 0;

// Increment current hours/minutes/seconds
void increment_clock() {
  if (ms_elapsed >= 1000) {
    clock_seconds++;
    ms_elapsed = 0;
  }
  
  if (clock_seconds >= 60) {
    clock_seconds = 0;
    clock_minutes++;
      
    if (clock_minutes >= 60) {
      clock_minutes = 0;
      clock_hours++;  

      if (clock_hours >= 24) {
        clock_hours = 0;
      }
    }
  }
}

// Display current hours/minutes/seconds
void display_clock() {
  // Define a string of 16 positions
  char current_time[17];
  
  int converted_hour = clock_hours;
  
  // Convert to 12h format
  if (!mode_24h) {
    if (clock_hours == 0) {
      converted_hour = 12;
    }
    else if (clock_hours > 12) {
      converted_hour = clock_hours - 12;
    }
  }
  
  // Formatting a string in HH:MM:SS format
  sprintf(current_time, "%02d:%02d:%02d", converted_hour, clock_minutes, clock_seconds);
  
  // Printing the string on the LCD
  lcd.setCursor(0, 0);
  lcd.print(current_time);
  
  if (!mode_24h) {
    if (clock_hours >= 12) {
      lcd.print("pm");
    }
    else {
      lcd.print("am");
    }
  }
  else {
    lcd.print("  ");
  }
}

// Display alarm hours/minutes/seconds
void display_alarm() {
  // Define a string of 16 positions
  char current_time[17];
  
  int converted_hour = alarm_hours;
  
  // Convert to 12h format
  if (!mode_24h) {
    if (alarm_hours > 12) {
      converted_hour = alarm_hours - 12;
    }
  }
  
  // Formatting a string in HH:MM:SS format
  sprintf(current_time, "%02d:%02d:%02d", converted_hour, alarm_minutes, alarm_seconds);
  
  // Printing the string on the LCD
  lcd.setCursor(0, 0);
  lcd.print(current_time);
  
  if (!mode_24h) {
    if (alarm_hours > 12) {
      lcd.print("pm");
    }
    else {
      lcd.print("am");
    }
  }
}

void display_mode() {
  lcd.setCursor(0, 1);
  
  switch (current_state) {
    case NORMAL:
        lcd.print("Alarm: ");
        if (alarm_enabled) {
            lcd.print("ON      ");
        }
        else {
            lcd.print("OFF     ");
        }
        break;
    
    case ALARM_TRIGGERED:
        lcd.print("Alarm triggered!");
        break;
    
    case SELECT_FORMAT:
        break;
    
    case ADJUST_HOUR:
        lcd.print("Adjust: Hour    ");
        break;
    
    case ADJUST_MINUTE:
        lcd.print("Adjust: Min     ");
        break;
    
    case ADJUST_SECOND:
        lcd.print("Adjust: Sec     ");
        break;
    
    case ALARM_HOUR:
        lcd.print("Alarm: Hour    ");
        break;
    
    case ALARM_MINUTE:
        lcd.print("Alarm: Min     ");
        break;
    
    case ALARM_SECOND:
        lcd.print("Alarm: Sec     ");
        break;
  }
}

// Project Setup
void setup() {
  // Set the number of columns and rows of the LCD
  lcd.begin(16, 2);
  
  // Set Buttons as Input
  pinMode(BTN01, INPUT_PULLUP);
  pinMode(BTN02, INPUT_PULLUP);
  pinMode(BTN03, INPUT_PULLUP);
  pinMode(BTN04, INPUT_PULLUP);
  
  // Set LED as Output
  pinMode(LED, OUTPUT);
}

// Project Actions
void loop() {
  display_mode();
  
  switch (current_state) {
    case NORMAL:
        // Output Logic
        increment_clock();
        display_clock();
        
        // Next State Logic
        if (alarm_enabled &&
            clock_seconds == alarm_seconds &&
            clock_minutes == alarm_minutes &&
            clock_hours == alarm_hours) {
            current_state = ALARM_TRIGGERED;
        }
        else if (digitalRead(BTN01) == LOW) {
            current_state = SELECT_FORMAT;
        }
        else if (digitalRead(BTN02) == LOW) {
            current_state = ADJUST_HOUR;
        }
        else if (digitalRead(BTN04) == LOW) {
            current_state = ALARM_HOUR;
        }
        
        delay(200);
        ms_elapsed += 200;
        break;
    
    case ALARM_TRIGGERED:
        delay(200);
        ms_elapsed += 200;
        increment_clock();
        display_clock();
    
        led_level = !led_level;
        digitalWrite(LED, led_level);
    
        if (digitalRead(BTN03) == LOW) {
            current_state = NORMAL;
        }
    
        tone(BUZZER, NOTE_A4, 150);
        delay(350);
        noTone(BUZZER);
    
        break;
    
    case SELECT_FORMAT:
        // Toggle the mode
        mode_24h = !mode_24h;
        display_clock();
    
        // Return to NORMAL
        current_state = NORMAL;
        break;
    
    case ADJUST_HOUR:
        display_clock();
    
        if (digitalRead(BTN03) == LOW) {
            clock_hours = (clock_hours + 1) % 24;
        }
    
        // Next State Logic
        else if (digitalRead(BTN02) == LOW) {
            current_state = ADJUST_MINUTE;
        }
        
        delay(200);
        break;
    
    case ADJUST_MINUTE:
        display_clock();
    
        if (digitalRead(BTN03) == LOW) {
            clock_minutes = (clock_minutes + 1) % 60;
        }
    
        // Next State Logic
        if (digitalRead(BTN02) == LOW) {
            current_state = ADJUST_SECOND;
        }
    
        delay(200);
        break;
    
    case ADJUST_SECOND:
        display_clock();
    
        if (digitalRead(BTN03) == LOW) {
            clock_seconds = (clock_seconds + 1) % 60;
        }
    
        // Next State Logic
        if (digitalRead(BTN02) == LOW) {
            current_state = NORMAL;
        }
    
        delay(200);
        break;
    
    case ALARM_HOUR:
        display_alarm();
        
    
        if (digitalRead(BTN03) == LOW) {
            alarm_hours = (alarm_hours + 1) % 24;
        }
    
        // Next State Logic
        else if (digitalRead(BTN04) == LOW) {
            current_state = NORMAL;
            alarm_enabled = false;
        }
        else if (digitalRead(BTN02) == LOW) {
            current_state = ALARM_MINUTE;
        }
    
        delay(200);
        ms_elapsed += 200;
        increment_clock();
        break;
    
    case ALARM_MINUTE:
        display_alarm();
    
        if (digitalRead(BTN03) == LOW) {
            alarm_minutes = (alarm_minutes + 1) % 60;
        }
    
        // Next State Logic
        else if (digitalRead(BTN04) == LOW) {
            current_state = NORMAL;
            alarm_enabled = false;
        }
        else if (digitalRead(BTN02) == LOW) {
            current_state = ALARM_SECOND;
        }
    
        delay(200);
        ms_elapsed += 200;
        increment_clock();
        break;
    
    case ALARM_SECOND:
        display_alarm();
    
        if (digitalRead(BTN03) == LOW) {
            alarm_seconds = (alarm_seconds + 1) % 60;
        }
    
        // Next State Logic
        else if (digitalRead(BTN04) == LOW) {
            current_state = NORMAL;
            alarm_enabled = false;
        }
        else if (digitalRead(BTN02) == LOW) {
            current_state = NORMAL;
            alarm_enabled = true;
        }
    
        delay(200);
        ms_elapsed += 200;
        increment_clock();
        break;
  }
}
