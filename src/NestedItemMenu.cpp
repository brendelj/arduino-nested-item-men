#include "NestedItemMenu.h"

NestedItemMenu::NestedItemMenu(LiquidCrystal_I2C& lcd,
                               uint8_t columns,
                               uint8_t rows,
                               uint8_t titleRow,
                               uint8_t firstItemRow)
    : lcd_(lcd),
      columns_(columns),
      rows_(rows),
      titleRow_(titleRow),
      firstItemRow_(firstItemRow),
      visibleRows_(rows > firstItemRow ? rows - firstItemRow : 0),
      cursorCharIndex_(0),
      active_(false),
      dirty_(true),
      depth_(0) {}

void NestedItemMenu::begin(const char* rootTitle,
                           const MenuItem* rootItems,
                           uint8_t rootItemCount,
                           uint8_t cursorCharIndex) {
  cursorCharIndex_ = cursorCharIndex;
  depth_ = 0;
  levels_[0] = {rootTitle, rootItems, rootItemCount, 0, 0};
  dirty_ = true;
}

void NestedItemMenu::open() {
  active_ = true;
  dirty_ = true;
  draw(true);
}

void NestedItemMenu::close() {
  active_ = false;
  dirty_ = true;
}

bool NestedItemMenu::isActive() const {
  return active_;
}

NestedItemMenu::Level& NestedItemMenu::currentLevel() {
  return levels_[depth_];
}

const NestedItemMenu::Level& NestedItemMenu::currentLevel() const {
  return levels_[depth_];
}

void NestedItemMenu::rotate(int steps) {
  if (!active_ || steps == 0 || currentLevel().count == 0) return;

  Level& level = currentLevel();
  int next = static_cast<int>(level.selected) + steps;
  while (next < 0) next += level.count;
  while (next >= level.count) next -= level.count;
  level.selected = static_cast<uint8_t>(next);

  if (level.selected < level.top) {
    level.top = level.selected;
  } else if (level.selected >= level.top + visibleRows_) {
    level.top = level.selected - visibleRows_ + 1;
  }

  dirty_ = true;
  draw();
}

void NestedItemMenu::select() {
  if (!active_ || currentLevel().count == 0) return;

  const MenuItem& item = currentLevel().items[currentLevel().selected];
  switch (item.type) {
    case MenuItemType::Action:
      if (item.action) item.action();
      dirty_ = true;
      break;
    case MenuItemType::Submenu:
      enterSubmenu(item);
      break;
    case MenuItemType::Back:
      back();
      return;
  }
  draw();
}

void NestedItemMenu::back() {
  if (!active_) return;
  if (depth_ > 0) {
    depth_--;
    dirty_ = true;
    draw(true);
  } else {
    close();
  }
}

void NestedItemMenu::enterSubmenu(const MenuItem& item) {
  if (!item.children || item.childCount == 0 || depth_ + 1 >= MAX_DEPTH) return;
  depth_++;
  levels_[depth_] = {item.label, item.children, item.childCount, 0, 0};
  dirty_ = true;
}

void NestedItemMenu::draw(bool force) {
  if (!active_ || (!dirty_ && !force)) return;

  if (force) lcd_.clear();
  const Level& level = currentLevel();

  lcd_.setCursor(0, titleRow_);
  printPadded(level.title ? level.title : "Menu", columns_);

  for (uint8_t rowOffset = 0; rowOffset < visibleRows_; rowOffset++) {
    uint8_t lcdRow = firstItemRow_ + rowOffset;
    uint8_t itemIndex = level.top + rowOffset;
    clearRow(lcdRow);
    if (itemIndex >= level.count) continue;

    lcd_.setCursor(0, lcdRow);
    if (itemIndex == level.selected) {
      lcd_.write(cursorCharIndex_);
    } else {
      lcd_.print(' ');
    }

    const MenuItem& item = level.items[itemIndex];
    uint8_t suffixWidth = item.type == MenuItemType::Submenu ? 1 : 0;
    uint8_t textWidth = columns_ > 1 + suffixWidth ? columns_ - 1 - suffixWidth : 0;
    printPadded(item.label ? item.label : "", textWidth);
    if (item.type == MenuItemType::Submenu && columns_ > 1) lcd_.print('>');
  }

  dirty_ = false;
}

void NestedItemMenu::clearRow(uint8_t row) {
  lcd_.setCursor(0, row);
  for (uint8_t i = 0; i < columns_; i++) lcd_.print(' ');
}

void NestedItemMenu::printPadded(const char* text, uint8_t width) {
  uint8_t written = 0;
  if (text) {
    while (*text && written < width) {
      lcd_.print(*text++);
      written++;
    }
  }
  while (written++ < width) lcd_.print(' ');
}

uint8_t NestedItemMenu::selectedIndex() const {
  return currentLevel().selected;
}

const MenuItem* NestedItemMenu::selectedItem() const {
  const Level& level = currentLevel();
  return level.count ? &level.items[level.selected] : nullptr;
}
