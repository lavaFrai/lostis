#include <GyverButton.h>
#include <GyverWDT.h>
#include <buildTime.h>


#define DEBUG 1
#define ERROR_WAIT_TIME 5000

#ifdef DEBUG 1
  #define log(x) Serial.println(x)
#else
  #define log(x)
#endif


byte render_menu();
byte show_info();
byte power_off();
byte tick();
byte show_loading();
byte show_error(String process_id, byte error_code);


struct menu_item 
{
  String name;
  byte (*function)();
};

menu_item menu[] = 
{
  {"Об устройстве", show_info},
  {"Выключить", power_off}
};



void setup() 
{
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  render_menu(menu);
}

void loop() 
{
  
}

byte render_menu(menu_item *list) 
{
  
  return 0;
}

byte tick()
{
  
}

byte show_loading() 
{
  
  return 0;
}

byte show_error(String process_id, byte error_code)
{
  if (error_code == 0) return 0;
  
  delay(ERROR_WAIT_TIME);
  power_off();
}

byte show_info() 
{
  
  return 0;
}

byte power_off() 
{
  
  return 0;
}
