# Arduino Nested Item Menu

A small reusable nested menu library for Arduino character LCD projects. Input is deliberately routed by the sketch, so the same encoder and button can also be shared with a rotary keyboard, numeric spinner, or other editor.

## Features

- Nested submenus up to eight levels
- Action callbacks
- Back items and root close behavior
- Scrolling on 20x4 and other character LCD sizes
- Wrap-around encoder navigation
- No ownership of encoder or button event handlers
- AVR-friendly static storage; no dynamic allocation

## Installation

Copy this repository into the Arduino libraries folder as `arduino-nested-item-men`, or use **Sketch > Include Library > Add .ZIP Library** with a ZIP of this repository.

Dependencies:

- `LiquidCrystal_I2C`

## Menu declaration

```cpp
void startAction();
void stopAction();

const MenuItem controlItems[] = {
  MENU_ACTION("Start", startAction),
  MENU_ACTION("Stop", stopAction),
  MENU_BACK("Back")
};

const MenuItem rootItems[] = {
  MENU_SUBMENU("Controller", controlItems),
  MENU_ACTION("About", showAbout)
};
```

## Input routing

```cpp
menu.rotate(1);   // clockwise step
menu.rotate(-1);  // counter-clockwise step
menu.select();    // click
menu.back();      // long press or double click
```

This explicit routing is important when a sketch also uses `RotaryKeyboard`: the sketch decides whether button and encoder events go to the menu or keyboard.
