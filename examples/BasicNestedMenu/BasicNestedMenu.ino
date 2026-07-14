#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <EncoderButton.h>
#include <NestedItemMenu.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
Encoder encoder(A0, A1);
EncoderButton button(A2);
NestedItemMenu menu(lcd);

long lastEncoder = 0;
byte cursorChar[8] = {B00000, B00100, B00110, B11111, B00110, B00100, B00000, B00000};

void showStarted() {
  menu.close();
  lcd.clear();
  lcd.print("Controller started");
}

void showStopped() {
  menu.close();
  lcd.clear();
  lcd.print("Controller stopped");
}

void showAbout() {
  menu.close();
  lcd.clear();
  lcd.print("Nested Menu v1.0");
}

const MenuItem controllerItems[] = {
  MENU_ACTION("Start", showStarted),
  MENU_ACTION("Stop", showStopped),
  MENU_BACK("Back")
};

const MenuItem rootItems[] = {
  MENU_SUBMENU("Controller", controllerItems),
  MENU_ACTION("About", showAbout)
};

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, cursorChar);

  menu.begin("Main Menu", rootItems, sizeof(rootItems) / sizeof(rootItems[0]), 0);
  menu.open();

  button.setClickHandler([](EncoderButton&) { menu.select(); });
  button.setLongPressHandler([](EncoderButton&) { menu.back(); });
}

void loop() {
  button.update();
  long current = encoder.read();
  long delta = current - lastEncoder;
  if (abs(delta) >= 4) {
    int steps = delta / 4;
    lastEncoder += steps * 4;
    menu.rotate(steps);
  }
}
