#include <buildTime.h>
#include <directADC.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <avr/boot.h>
#include "inc/button.h"
#include "languages/language_pack.cpp"
#include <avr/sleep.h>
#include <avr/power.h>
#include <EEPROM.h>

#define VERSION "v0.5.0"

namespace defines {
#define forever while(1)
#define DEBUG 1
#define ERROR_WAIT_TIME 5000
#define DOWN_BUTTON_PIN 7
#define UP_BUTTON_PIN 9
#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 5
#define OK_BUTTON_PIN 6
#define BACK_BUTTON_PIN 8
#define PEZO_PIN 3
byte ADC_PIN = A2;
byte VOLTMETER_PIN = A2;
byte RESISTANCE_PIN = A2;
byte TESTER_PIN1 = A3;
byte TESTER_PIN2 = A1;
#define V_DIV_R1 17.7
#define V_DIV_R2 2.54
#define V_ADC_SHIFT 0.1
#define V_ADC_SHIFT2 1.005
#define main_font u8g2_font_profont11_tr

#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#ifdef DEBUG
#define log(x) Serial.println(x)
#else
#define log(x)
#endif
} using namespace defines;

namespace global {
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0);
//U8G2_SH1106_128X64_NONAME_1_4W_HW_SPI display(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
} using namespace global;

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void applySettings()
{
  display.setContrast((uint8_t)(EEPROM.read(3) * 2.55));
}

namespace apps {
byte test()
{
  return 0x04;
}

byte sysinfo()
{
  button down(DOWN_BUTTON_PIN), up(UP_BUTTON_PIN), left(LEFT_BUTTON_PIN), right(RIGHT_BUTTON_PIN), ok(OK_BUTTON_PIN), back(BACK_BUTTON_PIN);
  
  _delay_ms(200);
  while (!ok.click() && !back.click())
  {
    display.firstPage();
    do {
      display.setCursor(0, 10);
      display.print(F("lostis OS "));
      display.print(F(VERSION));
      display.setCursor(0, 20);
      display.print(F(__DATE__));
      display.print(F("  "));
      display.print(F(__TIME__));
      display.setCursor(0, 30);
      display.print(F("RAM alloc: "));
      display.print((2048-freeRam()));
      display.print(F("b("));
      display.print((int)((float)(2048-freeRam())*100/2048.0));
      display.print(F("%)"));
      display.setCursor(0, 40);
      display.print(F("Startup time: "));
      display.print((millis() / 1000) / 60);
      display.print(F("m "));
      display.print((millis() / 1000) % 60);
      display.print(F("s"));
      display.setCursor(0, 50);
      display.print(F("CPU UID (hex view): "));
      display.setCursor(0, 60);
      for (uint8_t i = 14; i < 24; i += 1) {
          display.print(String(boot_signature_byte_get(i), HEX));
      }
    } while (display.nextPage());
    
  }
  while (!digitalRead(OK_BUTTON_PIN));
  _delay_ms(10);
  return 0;
}

byte parser()
{
  
  return 0;
}

enum {BYTE_VALUE, PERCENTAGE_TYPE, BOOL_VALUE, QPERCENTAGE_TYPE};

const struct setting{const char* name; int EEptr; uint8_t type;} settings[] = {
  {"Contrast ", 3, QPERCENTAGE_TYPE},
  {"test_BOOL", 4, BOOL_VALUE},
  {"test_PERC", 5, PERCENTAGE_TYPE},
  {"test_QPERC", 6, QPERCENTAGE_TYPE},
  {"test_BYTE", 7, BYTE_VALUE}
};

byte config()
{
  button down(DOWN_BUTTON_PIN), up(UP_BUTTON_PIN), left(LEFT_BUTTON_PIN), right(RIGHT_BUTTON_PIN), ok(OK_BUTTON_PIN), back(BACK_BUTTON_PIN);
  uint8_t menu_shift = 0, menu_select = 0, menu_len = sizeof(settings) / sizeof(setting), menu_selected_pos = 0;
  
  _delay_ms(200);
  while (!ok.click() && !back.click())
  {
    if (down.click()) menu_select++;
    if (up.click()) menu_select--;
    if (ok.click()) {
      return 0x12;
    }
    if (menu_select == menu_len) menu_select = 0;
    if (menu_select == 255) menu_select = menu_len - 1;
    if (menu_select - menu_shift == 4) menu_shift++;
    if (menu_shift - menu_select == 1) menu_shift--;
    if (!menu_shift && (menu_select == menu_len - 1)) menu_shift = menu_len - 4;
    if (menu_shift == menu_len - 4 && menu_select == 0) menu_shift = 0;
    display.firstPage();
    do {
      display.setFont(main_font);
      display.setCursor(24, 7);
      display.print(F("Configuration"));
      display.drawHLine(0, 8, 128);
      
      FOR_i(0, 4)
      {
        display.setCursor(0, 21 + 12 * i);
        if (i + menu_shift == menu_select)
        {
          display.print(F(" > "));
        }
        else
        {
          display.print(F("   "));
        }
        display.print(settings[i+menu_shift].name);
        // Рисуем значение
        switch (settings[i+menu_shift].type)
        {
          case (PERCENTAGE_TYPE):
            if (EEPROM.read(settings[i+menu_shift].EEptr) < 100 && EEPROM.read(settings[i+menu_shift].EEptr) > 9)
              display.setCursor(96, 21 + 12 * i);
            else if (EEPROM.read(settings[i+menu_shift].EEptr) < 10)
              display.setCursor(102, 21 + 12 * i);
            else
              display.setCursor(90, 21 + 12 * i);
            display.print(EEPROM.read(settings[i+menu_shift].EEptr));
            display.print(F("%"));
            break;
          case (QPERCENTAGE_TYPE):
            if (EEPROM.read(settings[i+menu_shift].EEptr) < 100 && EEPROM.read(settings[i+menu_shift].EEptr) > 9)
              display.setCursor(96, 21 + 12 * i);
            else if (EEPROM.read(settings[i+menu_shift].EEptr) < 10)
              display.setCursor(102, 21 + 12 * i);
            else
              display.setCursor(90, 21 + 12 * i);
            display.print(EEPROM.read(settings[i+menu_shift].EEptr));
            display.print(F("%"));
            break;
          case (BYTE_VALUE):
            if (EEPROM.read(settings[i+menu_shift].EEptr) < 100 && EEPROM.read(settings[i+menu_shift].EEptr) > 9)
              display.setCursor(102, 21 + 12 * i);
            else if (EEPROM.read(settings[i+menu_shift].EEptr) < 10)
              display.setCursor(108, 21 + 12 * i);
            else
              display.setCursor(96, 21 + 12 * i);
            display.print(EEPROM.read(settings[i+menu_shift].EEptr));
            break;
          case (BOOL_VALUE):
            if (EEPROM.read(settings[i+menu_shift].EEptr))
              display.drawBox(106, 14 + 12 * i, 7, 7);
            else
              display.drawFrame(106, 14 + 12 * i, 7, 7);
            break;
        }
        applySettings();
        display.setCursor(109, 21 + 12 * i);
        if (i + menu_shift == menu_select)
        {
          display.print(F(" < "));
        }
        else
        {
          display.print(F("   "));
        }
      }

      switch (settings[menu_select].type)
        {
          case (PERCENTAGE_TYPE):
            if (left.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) - 1);
            if (right.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) + 1);
            if (EEPROM.read(settings[menu_select].EEptr) == 255) EEPROM.write(settings[menu_select].EEptr, 100);
            if (EEPROM.read(settings[menu_select].EEptr) > 100) EEPROM.write(settings[menu_select].EEptr, 0);
            break;
          case (QPERCENTAGE_TYPE):
            if (left.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) - 10);
            if (right.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) + 10);
            if (EEPROM.read(settings[menu_select].EEptr) > 200) EEPROM.write(settings[menu_select].EEptr, 100);
            if (EEPROM.read(settings[menu_select].EEptr) > 100) EEPROM.write(settings[menu_select].EEptr, 0);
            break;
          case (BYTE_VALUE):
            if (left.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) - 1);
            if (right.click()) EEPROM.write(settings[menu_select].EEptr, EEPROM.read(settings[menu_select].EEptr) + 1);
            break;
          case (BOOL_VALUE):
            if (left.click()) EEPROM.write(settings[menu_select].EEptr, !EEPROM.read(settings[menu_select].EEptr));
            if (right.click()) EEPROM.write(settings[menu_select].EEptr, !EEPROM.read(settings[menu_select].EEptr));
            break;
        }
    } while (display.nextPage());
  }
  while (!digitalRead(OK_BUTTON_PIN));
  _delay_ms(10);
  
  return 0;
}

byte shutdown()
{
  display.setPowerSave(1);
  Serial.end();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
  Serial.begin(9600);
  display.setPowerSave(0);
}
}


namespace kernel {
button down(DOWN_BUTTON_PIN), up(UP_BUTTON_PIN), left(LEFT_BUTTON_PIN), right(RIGHT_BUTTON_PIN), ok(OK_BUTTON_PIN);
uint8_t menu_shift = 0, menu_select = 0, menu_len = 0, menu_selected_pos = 0;

void error_handler(uint8_t code)
{
  if (code)
  {
    display.firstPage();
    do {
      display.drawFrame(16, 16, 96, 32);
      display.setFont(main_font);
      display.setCursor(0, 10);
      display.print(F("Kernel:"));
      display.setCursor(18, 26);
      display.print(F("App finished"));
      display.setCursor(18, 36);
      display.print(F("with error code"));
      display.setCursor(18, 46);
      display.print(code < 0x10 ? F("0") : F(""));
      display.print(String(code, HEX));
      //display.print('h');
      display.setCursor(0, 63);
      display.print(F("reboot device (rst)"));
    } while (display.nextPage());
    while (1);
  }
}

struct menu_item
{
  char* name;
  byte (*function)();
};

menu_item menu[] =
{
  {"Apps (not ready)", apps::parser},
  {"Configuration", apps::config},
  {"Sysinfo", apps::sysinfo},
  {"Shutdown", apps::shutdown}
};

void boot()
{
  display.firstPage();
  do {
    display.setFont(main_font);
    display.setCursor(0, 8);
    display.print(F("Booting lostis kernel"));
  } while (display.nextPage());

  /* Подкачиваем вссе необходимые данные, вобщем грузимся */

  display.firstPage();
  do {
    display.setFont(main_font);
    display.setCursor(0, 8);
    display.print(F("Lostis kernel started"));
  } while (display.nextPage());
  return;
}

void run()
{
  menu_len = sizeof(menu) / sizeof(menu_item);
  while (1)
  {
    if (down.click()) menu_select++;
    if (up.click()) menu_select--;
    if (ok.click()) {
      error_handler(menu[menu_select].function()) ;
      return;
    }
    if (menu_select == menu_len) menu_select = 0;
    if (menu_select == 255) menu_select = menu_len - 1;
    if (menu_select - menu_shift == 4) menu_shift++;
    if (menu_shift - menu_select == 1) menu_shift--;
    if (!menu_shift && (menu_select == menu_len - 1)) menu_shift = menu_len - 4;
    if (menu_shift == menu_len - 4 && menu_select == 0) menu_shift = 0;
    display.firstPage();
    do {
      display.setFont(main_font);
      display.setCursor(37, 7);
      display.print(F("Main menu"));
      display.drawHLine(0, 8, 128);

      FOR_i(0, 4)
      {
        display.setCursor(0, 21 + 12 * i);
        if (i + menu_shift == menu_select)
        {
          display.print(F(" > "));
          display.print(menu[menu_shift + i].name);
          display.setCursor(109, 21 + 12 * i);
          display.print(F(" < "));
        }
        else
        {
          display.print(F("   "));
          display.print(menu[menu_shift + i].name);
          display.setCursor(109, 21 + 12 * i);
          display.print(F("   "));
        }
      }
    } while (display.nextPage());
  }
  return;
}
} using namespace kernel;


void setup()
{

  display.begin();
  Serial.begin(9600);
  kernel::boot();
  applySettings();
}

void loop()
{
  kernel::run();
}
