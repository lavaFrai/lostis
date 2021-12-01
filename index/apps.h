
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

byte config()
{
  button down(DOWN_BUTTON_PIN), up(UP_BUTTON_PIN), left(LEFT_BUTTON_PIN), right(RIGHT_BUTTON_PIN), ok(OK_BUTTON_PIN), back(BACK_BUTTON_PIN);
  
  _delay_ms(200);
  while (!ok.click() && !back.click())
  {
    display.firstPage();
    do {
      
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
