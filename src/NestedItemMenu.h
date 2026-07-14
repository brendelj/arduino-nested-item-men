#ifndef NESTED_ITEM_MENU_H
#define NESTED_ITEM_MENU_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class NestedItemMenu;

using MenuAction = void (*)();

enum class MenuItemType : uint8_t {
  Action,
  Submenu,
  Back
};

struct MenuItem {
  const char* label;
  MenuItemType type;
  MenuAction action;
  const MenuItem* children;
  uint8_t childCount;
};

class NestedItemMenu {
public:
  NestedItemMenu(LiquidCrystal_I2C& lcd,
                 uint8_t columns = 20,
                 uint8_t rows = 4,
                 uint8_t titleRow = 0,
                 uint8_t firstItemRow = 1);

  void begin(const char* rootTitle,
             const MenuItem* rootItems,
             uint8_t rootItemCount,
             uint8_t cursorCharIndex = 0);

  void open();
  void close();
  bool isActive() const;

  void rotate(int steps);
  void select();
  void back();
  void draw(bool force = false);

  uint8_t selectedIndex() const;
  const MenuItem* selectedItem() const;

private:
  static const uint8_t MAX_DEPTH = 8;

  struct Level {
    const char* title;
    const MenuItem* items;
    uint8_t count;
    uint8_t selected;
    uint8_t top;
  };

  LiquidCrystal_I2C& lcd_;
  uint8_t columns_;
  uint8_t rows_;
  uint8_t titleRow_;
  uint8_t firstItemRow_;
  uint8_t visibleRows_;
  uint8_t cursorCharIndex_;
  bool active_;
  bool dirty_;
  Level levels_[MAX_DEPTH];
  uint8_t depth_;

  Level& currentLevel();
  const Level& currentLevel() const;
  void enterSubmenu(const MenuItem& item);
  void clearRow(uint8_t row);
  void printPadded(const char* text, uint8_t width);
};

#define MENU_ACTION(labelText, callbackFn) \
  { labelText, MenuItemType::Action, callbackFn, nullptr, 0 }

#define MENU_SUBMENU(labelText, childArray) \
  { labelText, MenuItemType::Submenu, nullptr, childArray, static_cast<uint8_t>(sizeof(childArray) / sizeof(childArray[0])) }

#define MENU_BACK(labelText) \
  { labelText, MenuItemType::Back, nullptr, nullptr, 0 }

#endif
