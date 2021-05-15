#include <GyverButton.h>
#include <GyverPower.h>
#include <GyverWDT.h>
#include <buildTime.h>
#include <GyverTimers.h>
#include <GyverOLED.h>
#include "languages/language_pack.cpp"

#define forever while(1)
#define DEBUG 1
#define ERROR_WAIT_TIME 5000
#define DOWN_BUTTON_PIN 10
#define UP_BUTTON_PIN 9
#define LEFT_BUTTON_PIN 4
#define RIGHT_BUTTON_PIN 5
#define OK_BUTTON_PIN 6
#define BACK_BUTTON_PIN 7
#define Q_RETURN if (up_button.isSingle()) return 0; \
                 if (down_button.isSingle()) return 0; \
                 if (ok_button.isSingle()) return 0; \
                 if (left_button.isSingle()) return 0; \
                 if (right_button.isSingle()) return 0

#define FOR_i(from, to) for(int i = (from); i < (to); i++)

#ifdef DEBUG
  #define log(x) Serial.println(x)
#else
  #define log(x)
#endif


byte show_info();
byte power_off();
byte ext_module();
byte tick();
byte settings();
byte show_loading();
byte show_error(String process_id, byte error_code);

byte adc_vue();
byte voltmeter();


struct menu_item 
{
  String name;
  byte (*function)();
};


menu_item menu[] = 
{
  {VOLTMETER_T, voltmeter},
  {ADC_VIEWER_T, adc_vue},
  {SETTINGS_T, settings},
  {ABOUT_T, show_info},
  {EXTERNAL_MODULE, ext_module},
  {SHUTDOWN_T, power_off},
};

GyverOLED OLED;
GButton down_button(DOWN_BUTTON_PIN);
GButton up_button(UP_BUTTON_PIN);
GButton left_button(LEFT_BUTTON_PIN);
GButton right_button(RIGHT_BUTTON_PIN);
GButton ok_button(OK_BUTTON_PIN);
GButton back_button(BACK_BUTTON_PIN);

void setup()
{
  OLED.init(OLED128x64);
  show_loading();
  Watchdog.enable(RESET_MODE, WDT_PRESCALER_1024);
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  Timer2.setFrequency(162);
  Timer2.enableISR(CHANNEL_A);
  Timer2.restart();
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  while (millis() < 2000) delay(10);
}

void loop() 
{
  Watchdog.reset();
  render_menu(menu, MAIN_MENU_T, sizeof(menu) / 8);
  
}

ISR(TIMER2_A) 
{
  down_button.tick();
  up_button.tick();
  left_button.tick();
  right_button.tick();
  ok_button.tick();
  back_button.tick();
  // if (up_button.isSingle()) log("btnUp");
  // if (down_button.isSingle()) log("btnDown");
}

byte render_menu(menu_item *list, String header, byte menu_len) 
{
  ok_button.resetStates();
  left_button.resetStates();
  right_button.resetStates();
  up_button.resetStates();
  down_button.resetStates();
  OLED.clear();
  uint8_t k = 0, f = 0, l = menu_len;
  forever {
    OLED.home();
    OLED.scale1X();
    OLED.inverse(0);
    // log(header.length());
    // log(header);
    OLED.setCursor((20 - header.length())/2, 0);
    OLED.print(header);
    OLED.line(0, 10, 128, 10);
    while (f + 2 < k) f++;
    while (f > k) f--;
    for (int i = 0; i < 3; i++) {
      if (k == i + f) {
        OLED.inverse(1);
        OLED.setCursor(0, (i + 1) * 10);
        OLED.print(" > ");
        OLED.print(list[i + f].name);
        for (int j = 0; j < 16 - list[i + f].name.length(); j++) {
          OLED.print(" ");
        }
        OLED.print("<");
      }
      else {
        OLED.inverse(0);
        OLED.setCursor(0, (i + 1) * 10);
        OLED.print(" ~ ");
        OLED.print(list[i + f].name);
        for (int j = 0; j < 16 - list[i + f].name.length(); j++) {
          OLED.print(" ");
        }
        OLED.print("~");
      }
    }
    forever
    {
      Watchdog.reset();
      if (up_button.isPress() || up_button.isHold()) {
        log("UP");
        if (k == 0) k = l - 1;
        else k--;
        break;
      }
      if (down_button.isPress() || down_button.isHold()) {
        log("DOWN");
        if (k == l - 1) k = 0;
        else k++;
        break;
      }
      if (ok_button.isSingle()) {
        ok_button.resetStates();
        left_button.resetStates();
        right_button.resetStates();
        up_button.resetStates();
        down_button.resetStates();
        log("OK");
        OLED.clear();
        list[k].function();
        ok_button.resetStates();
        left_button.resetStates();
        right_button.resetStates();
        up_button.resetStates();
        down_button.resetStates();
        OLED.clear();
        break;
        // return 0;
      }
    }
  }
  return 0;
}

byte tick()
{
  Watchdog.reset();
  return 0;
}

byte show_loading() 
{
  OLED.clear();
  OLED.home();
  OLED.setCursor(1, 3);

  OLED.scale2X();
  OLED.print(LOADING_T);
  OLED.print(".");
  delay(200);
  OLED.print(".");
  delay(200);
  OLED.print(".");
  OLED.inverse(0);

  //OLED.line(0, 0, 20, 20);
  //OLED.line(20, 20, 0, 20);
  return 0;
}

byte show_error(String process_id, byte error_code)
{
  if (error_code == 0) return 0;
  
  delay(ERROR_WAIT_TIME);
  power_off();
}

byte ext_module() 
{

  return 0;
}

byte show_info() 
{
  OLED.inverse(0);
  OLED.clear();
  OLED.home();
  OLED.setCursor(4, 1);

  OLED.scale2X();
  OLED.print(F("LOSTIS"));
  OLED.scale1X();
  OLED.setCursor(4, 3);
  OLED.print(F("the  project"));
  OLED.setCursor(0, 6);
  OLED.print(F("See more on this:"));
  OLED.setCursor(0, 7);
  OLED.print(F("http://clck.ru/UpLZT"));
  forever {
    Q_RETURN;
    tick();
  }
  return 0;
}

byte draw_value(String header, String title, int val, int from_val, int to_val) {
  // OLED.clear();
  val = constrain(val, from_val, to_val);
  OLED.scale1X();
  OLED.inverse(0);
  OLED.setCursor((20 - header.length())/2, 0);
  OLED.print(header);
  OLED.line(0, 10, 128, 10);
  OLED.scale2X();
  title += String(val);
  OLED.setCursor(0, 4);
  OLED.print(title);
  OLED.print("    ");
  // OLED.setCursor(0, 7);
  // OLED.scale1X();
  // OLED.print("                     ");
  // val = map(val, from_val, to_val, 0, 120);
  // log(val);
  // OLED.line(0, 63, val, 63);
}

byte draw_float_value(String header, String title, float val) {
  OLED.scale1X();
  OLED.inverse(0);
  OLED.setCursor((20 - header.length())/2, 0);
  OLED.print(header);
  OLED.line(0, 10, 128, 10);
  OLED.scale2X();
  title += String(val);
  OLED.setCursor(0, 4);
  OLED.print(title);
  OLED.print("    ");
  // OLED.setCursor(0, 7);
  // OLED.scale1X();
  // OLED.print("                     ");
  // val = map(val, from_val, to_val, 0, 120);
  // log(val);
  // OLED.line(0, 63, val, 63);
}

byte adc_vue() {
  OLED.clear();
  forever {
    Q_RETURN;
    draw_value(F("ADC data"), F("ADC: "), analogRead(A2), 0, 1024);
    tick();
  }
}

byte voltmeter() {
  OLED.clear();
  forever {
    Q_RETURN;
    draw_float_value(F("Voltage"), F("V: "), (float)analogRead(A2) / 204.8);
    tick();
  }
}

byte settings() 
{
  
  return 0;
}

byte power_off() 
{
  Serial.end();
  Watchdog.disable();
  power.sleep(SLEEP_FOREVER);
  return 0;
}
