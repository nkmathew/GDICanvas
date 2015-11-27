/*!
 * \file Canvas.cxx
 **/

// Created: Monday, April 21, 2014

#include "./Canvas.h"
using namespace GCanvas;

// ~~~~~~~~~~~~~~~~~~~~~~~~~[ Free functions ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool GCanvas::shiftKeyDown() {
  return (GetKeyState(VK_SHIFT) & 0x80000000) != 0;
}

bool GCanvas::altKeyDown() {
  return (GetKeyState(VK_MENU) & 0x80000000) != 0;
}

bool GCanvas::ctrlKeyDown() {
  return (GetKeyState(VK_CONTROL) & 0x80000000) != 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Canvas::maximize() {
  ShowWindow(winHandle, SW_MAXIMIZE);
}

void Canvas::minimize() {
  ShowWindow(winHandle, SW_MINIMIZE);
}

HWND Canvas::handle() {
  return winHandle;
}

bool Canvas::kill() {
  return DestroyWindow(winHandle);
}

void Canvas::background(int red, int green, int blue) {
  HBRUSH brush = CreateSolidBrush(RGB(red, green, blue));
  SetClassLongPtr(winHandle, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
  InvalidateRect(winHandle, NULL, TRUE);
}

void Canvas::background(std::string colorString) {
  colorString = Colors::hexValue(colorString);
  Colors::RGBValue rgb = Colors::hexToRGB(colorString);
  background(rgb.red, rgb.green, rgb.blue);
}

void Canvas::refreshWindow() {
  POINT size = windowSize();
  resizeWindow(size.x, size.y);
}

void Canvas::noMinimize() {
  unsigned style = GetWindowLong(winHandle, GWL_STYLE);
  SetWindowLongPtr(winHandle, GWL_STYLE, style & (~WS_MINIMIZEBOX));
  refreshWindow();
}

void Canvas::noMaximize() {
  unsigned style = GetWindowLong(winHandle, GWL_STYLE);
  SetWindowLongPtr(winHandle, GWL_STYLE, style & (~WS_MAXIMIZEBOX));
  refreshWindow();
}

void Canvas::sleep(int millSecs) {
  Sleep(millSecs);
}

GS::ShapeType Canvas::shapeType(int id) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == id) {
      return shape->type();
    }
  }
  return GS::INVALID_SHAPE;
}

bool Canvas::addHandler(Event event, const std::string &keyStr) {
  if (event.eventType == INVALID_EVENT) {
    return false;
  }
  int key = virtualKeys[keyStr];
  if (key) {
    if ((key != VK_LBUTTON) && (key != VK_RBUTTON) && (key != VK_MBUTTON)) {
      // Only set the key if it's a keyboard event.
      // VK_LBUTTON = 1; VK_RBUTTON = 2; VK_MBUTTON = 4
      event.keyToHandle = key;
    }
    events[event.eventType].push_back(event);
  }
  return key;
}

bool Canvas::unbind(const std::string &eventString,
                    int shapeID) {
  std::string keyStr("");
  EventType type = parseEventString(eventString, &keyStr);
  std::vector<Event>::iterator iter = events[type].begin();
  bool handlerExists = false;
  while (iter != events[type].end()) {
    if (events[type].size() == 0) {
      break;
    }
    if (iter->shapeID == shapeID) {
      events[type].erase(iter++);
      handlerExists = true;
      continue;
    }
    ++iter;
  }
  return handlerExists;
}

bool Canvas::unbind(const std::string &eventString,
                    const std::string &tag) {
  std::string keyStr("");
  EventType type = parseEventString(eventString, &keyStr);
  std::vector<Event>::iterator iter = events[type].begin();
  bool handlerExists = false;
  while (iter != events[type].end()) {
    if (events[type].size() == 0) {
      break;
    }
    if (iter->shapeTag == tag) {
      events[type].erase(iter++);
      handlerExists = true;
      continue;
    }
    ++iter;
  }
  return handlerExists;
}

bool Canvas::trackMouse() {
  TRACKMOUSEEVENT tme;
  tme.cbSize = sizeof(tme);
  tme.dwFlags = TME_HOVER;
  tme.dwHoverTime = HOVER_DEFAULT;
  tme.hwndTrack = winHandle;
  return TrackMouseEvent(&tme);
}

bool Canvas::callHandlers(EventType type, int key) {
  bool called = false;
  for (const Event &event : events[type]) {
    // Keyboard event
    int id = event.shapeID;
    std::string tag = event.shapeTag;
    if ((type == TIMER) && (event.timerID == key)) {
      Mouse mouse(winHandle);
      event.handler->handle(mouse);
      called = true;
    } else if ((type == LEFT_CLICK) ||
               (type == CTRL_LEFT_CLICK) ||
               (type == ALT_LEFT_CLICK) ||
               (type == RIGHT_CLICK) ||
               (type == HOVER) ||
               (type == WHEEL_ROLL) ||
               (type == WHEEL_CLICK)) {
      int delta = key;
      Mouse mouse(winHandle, delta);
      if ((id == -1) && (tag == "")) {
        // An unbound mouse event handler.
        event.handler->handle(mouse);
        called = true;
        continue;
      }
      // Mouse event
      auto shapeList_ = shapeList;
      for (const auto &shape : shapeList_) {
        int xPos = mouse.x();
        int yPos = mouse.y();
        if (((shape->shapeID == id)) ||
            ((shape->hasTag(tag)))) {
          if (shape->pointInShape(xPos, yPos)) {
            event.handler->handle(mouse);
            called = true;
          }
        }
      }
    } else if (event.keyToHandle == key) {
      Mouse mouse(winHandle);
      event.handler->handle(mouse);
      called = true;
    }
  }
  return called;
}

EventType Canvas::parseEventString(std::string eventString,
                                   std::string *keyString) {
  int length = eventString.length();
  if ((eventString.front() != '<') || (eventString.back() != '>')) {
    return INVALID_EVENT;
  }
  eventString = eventString.substr(1, length - 2);
  eventString += '-'; // Hack needed for the last token to be extracted
  std::string modifiers[3];
  std::string str = "";
  int i = 0;
  for (char ch : eventString) {
    if (ch == '-') {
      modifiers[i] = str;
      str = "";
      i++;
    } else {
      str = str + (char)tolower(ch);
    }
  }
  std::string first = modifiers[0];
  std::string second = modifiers[1];
  std::string third = modifiers[2];
  std::string temp;
  if (((second == "ctrl") && ((first == "shift") || (first == "mouse"))) ||
      (second == "alt")) {
    // It's easier to handle an ALT key combination in a WM_SYSKEYUP message
    // than in any other message using GetKeyState(VK_MENU)
    // This part turns a string like "<Ctrl-Alt-Q>" to "<Alt-Ctrl-Q>"
    temp = first;
    first = second;
    second = temp;
  }
  if (first == "timer") {
    *keyString = "<timer>";
    return TIMER;
  } else if (first == "hover") {
    *keyString = "<hover>";
    return HOVER;
  } else if (first == "ctrl") {
    if ((third == "1") && (second == "mouse")) {
      // <Ctrl-Mouse-1>
      *keyString = "<1>";
      return CTRL_LEFT_CLICK;
    } else if (second == "shift") {
      // <Ctrl-Shift-Q>
      *keyString = third;
      return CTRL_SHIFT_KEY;
    } else {
      // <Ctrl-Q>
      *keyString = second;
      return CTRL_KEY;
    }
  } else if (first == "alt") {
    if ((third == "1") && (second == "mouse")) {
      // <Alt-Mouse-1>
      *keyString = "<1>";
      return ALT_LEFT_CLICK;
    } else if (second == "shift") {
      // <Alt-Shift-Q>
      *keyString = third;
      return ALT_SHIFT_KEY;
    } else {
      // <Alt-Q>
      *keyString = second;
      return ALT_KEY;
    }
  } else if ((first == "mouse") && (second == "1") && (third == "")) {
    *keyString = "<1>";
    return LEFT_CLICK;
  } else if ((first == "mouse") && (second == "2") && (third == "")) {
    *keyString = "<2>";
    return RIGHT_CLICK;
  } else if ((first == "wheel") && (second == "roll") && (third == "")) {
    *keyString = "<3>";
    return WHEEL_ROLL;
  } else if ((first == "key") && (third == "")) {
    *keyString = second;
    return BARE_KEY;
  } else if ((first == "wheel") && (second == "click") && (third == "")) {
    *keyString = "<3>";
    return WHEEL_CLICK;
  }
  return INVALID_EVENT;
}

bool Canvas::isVisible(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      return shape->isShown();
    }
  }
  return false;
}

bool Canvas::hideShape(const std::string &tagName, bool visible) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->visibility(visible);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::hideShape(int shapeID, bool visible) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->visibility(visible);
      return true;
    }
  }
  return false;
}

bool Canvas::showShape(int shapeID) {
  return hideShape(shapeID, true);
}

bool Canvas::showShape(const std::string &tagName) {
  return hideShape(tagName, true);
}

bool Canvas::moveShape(const std::string &shapeTag, int xAmount, int yAmount) {
  bool foundAny = false;
  for (auto shape : shapeList) {
    if (shape->hasTag(shapeTag)) {
      shape->move(xAmount, yAmount);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::moveShape(int shapeID, int xAmount, int yAmount) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->move(xAmount, yAmount);
      return true;
    }
  }
  return false;
}

POINT Canvas::windowPos() {
  RECT rect;
  GetWindowRect(winHandle, &rect);
  return {rect.left, rect.top};
}

void Canvas::resizeWindow(int width, int height) {
  POINT currPos = windowPos();
  SetWindowPos(winHandle, 0, currPos.x, currPos.y, width, height,
               SWP_NOZORDER | SWP_NOACTIVATE);
}

void Canvas::moveWindow(int newX, int newY) {
  POINT currSize = windowSize();
  SetWindowPos(winHandle, 0, newX, newY, currSize.x, currSize.y,
               SWP_NOZORDER | SWP_NOACTIVATE);
}

POINT Canvas::windowSize() {
  RECT rect;
  GetWindowRect(winHandle, &rect);
  return {std::abs(rect.left - rect.right),
          std::abs(rect.top - rect.bottom)
         };
}

std::vector<int> Canvas::findEnclosed(int x1, int y1, int x2, int y2) {
  std::vector<int> items;
  for (const auto &shape : shapeList) {
    if (shape->shapeInRegion(Vec::Vec2D(x1, y1), Vec::Vec2D(x2, y2))) {
      items.push_back(shape->shapeID);
    }
  }
  return items;
}

std::vector<int> Canvas::findOverlapping(int x1, int y1, int x2, int y2) {
  std::vector<int> items;
  for (const auto &shape : shapeList) {
    if (shape->overlapsWithRegion(Vec::Vec2D(x1, y1), Vec::Vec2D(x2, y2))) {
      items.push_back(shape->shapeID);
    }
  }
  return items;
}

std::vector<int> Canvas::findAll() {
  return findWithTag("all");
}

std::vector<int> Canvas::findWithTag(const std::string &tag) {
  std::vector<int> items;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tag)) {
      items.push_back(shape->shapeID);
    }
  }
  return items;
}

std::vector<std::string> Canvas::getTags(int id) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == id) {
      return shape->tags();
    }
  }
  return {};
}

std::vector<int> Canvas::findAbove(int id) {
  std::vector<int> items;
  for (const auto &shape : shapeList) {
    if (shape->shapeID > id) {
      items.push_back(shape->shapeID);
    }
  }
  return items;
}

std::vector<int> Canvas::findBelow(int id) {
  std::vector<int> items;
  for (const auto &shape : shapeList) {
    if (shape->shapeID < id) {
      items.push_back(shape->shapeID);
    }
  }
  return items;
}

POINT Canvas::screenSize() {
  HMONITOR monitor = MonitorFromWindow(winHandle, MONITOR_DEFAULTTONEAREST);
  MONITORINFO info;
  info.cbSize = sizeof(MONITORINFO);
  GetMonitorInfo(monitor, &info);
  return {info.rcMonitor.right - info.rcMonitor.left,
          info.rcMonitor.bottom - info.rcMonitor.top
         };
}

Canvas::Canvas(int x, int y, int width, int height) {
  xPos = x;
  yPos = y;
  winWidth = width;
  winHeight = height;
}

Canvas::Canvas(int width, int height, const char *title) {
  winWidth = width;
  winHeight = height;
  windowTitle = title;
}

Canvas::Canvas(int x, int y) {
  xPos = x;
  yPos = y;
}

bool Canvas::coords(int shapeID, const std::vector<POINT> &newCoords) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->changeCoords(newCoords);
      return true;
    }
  }
  return false;
}

std::vector<POINT> Canvas::coords(int id) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == id) {
      return shape->coords();
    }
  }
  return {};
}

bool Canvas::raiseShape(const std::string &others, int target) {
  std::vector<int> shapes = findWithTag(others);
  bool res = false;
  for (int shape : shapes) {
    res |= raiseShape(shape, target);
  }
  return res;
}

bool Canvas::raiseShape(int first, int second) {
  std::shared_ptr<GS::Shape> shape;
  int firstPos = -1;
  int secondPos = -1;
  // Find the positions of the two elements
  for (unsigned i = 0; i < shapeList.size(); i++) {
    std::shared_ptr<GS::Shape> shp(shapeList[i]);
    firstPos = (shp->shapeID == first) ? i : firstPos;
    secondPos = (shp->shapeID == second) ? i : secondPos;
  }

  if (firstPos >= secondPos) {
    return false;
  }

  // Remove shape first
  for (unsigned i = 0; i < shapeList.size(); i++) {
    std::shared_ptr<GS::Shape> shp(shapeList[i]);
    if (shp->shapeID == first) {
      shape = std::shared_ptr<GS::Shape>(shp);
      shapeList.erase(shapeList.begin() + i);
      break;
    }
  }
  if (!shape) {
    return false;
  }

  // Insert it above second
  for (auto iter = shapeList.begin(); iter != shapeList.end(); iter++) {
    if ((*iter)->shapeID == second) {
      shapeList.insert(++iter, shape);
      return true;
    }
  }
  return false;
}

bool Canvas::lowerShape(const std::string &others, int target) {
  std::vector<int> shapes = findWithTag(others);
  bool res = false;
  for (int shape : shapes) {
    res |= lowerShape(shape, target);
  }
  return res;
}

bool Canvas::lowerShape(int first, int second) {
  return raiseShape(second, first);
}

std::string Canvas::icon() {
  return iconFile;
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void Canvas::fixBBoxCoord(int *x1, int *y1, int *x2, int *y2) {
  if (*x1 > *x2) {
    swap(x1, x2);
  }
  if (*y1 > *y2) {
    swap(y1, y2);
  }
}

bool Canvas::icon(const char *iconPath) {
  HICON hIcon, hIconSm;
  hIcon = (HICON)LoadImage(NULL, iconPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
  hIconSm = (HICON)LoadImage(NULL, iconPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

  if (hIconSm) {
    SendMessage(winHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(winHandle, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
    iconFile = iconPath;
    return true;
  }
  return false;
}

std::string Canvas::title() {
  return windowTitle;
}

bool Canvas::title(const char *title) {
  windowTitle = title;
  return SetWindowText(winHandle, title);
}

GS::Box Canvas::BBox(const std::vector<std::string> &tags) {
  float smallestX = FLT_MAX,
        smallestY = FLT_MAX;
  float largestX = -FLT_MAX,
        largestY = -FLT_MAX;
  for (const auto &shape : shapeList) {
    auto tagMatches = [&shape](const std::string & tag) {
      if (shape->hasTag(tag)) {
        return true;
      }
      return false;
    };
    if (std::any_of(tags.begin(), tags.end(), tagMatches)) {
      // The shape has at least one of the tags
      Vec::Vec2D topPoint(shape->topLeftCoord());
      Vec::Vec2D bottomPoint(shape->bottomRightCoord());
      smallestX = (topPoint.x < smallestX) ? topPoint.x : smallestX;
      smallestY = (topPoint.y < smallestY) ? topPoint.y : smallestY;
      largestX = (bottomPoint.x > largestX) ? bottomPoint.x : largestX;
      largestY = (bottomPoint.y > largestY) ? bottomPoint.y : largestY;
    }
  }
  return {smallestX, smallestY, largestX, largestY};
}

GS::Box Canvas::BBox(const std::vector<int> &shapes) {
  float smallestX = FLT_MAX,
        smallestY = FLT_MAX;
  float largestX = -FLT_MAX,
        largestY = -FLT_MAX;
  for (const auto &shape : shapeList) {
    auto hasID = [&shape](int id) {
      if (shape->shapeID == id) {
        return true;
      }
      return false;
    };
    if (std::any_of(shapes.begin(), shapes.end(), hasID)) {
      // The shape has at least one of the tags
      Vec::Vec2D topPoint(shape->topLeftCoord());
      Vec::Vec2D bottomPoint(shape->bottomRightCoord());
      smallestX = (topPoint.x < smallestX) ? topPoint.x : smallestX;
      smallestY = (topPoint.y < smallestY) ? topPoint.y : smallestY;
      largestX = (bottomPoint.x > largestX) ? bottomPoint.x : largestX;
      largestY = (bottomPoint.y > largestY) ? bottomPoint.y : largestY;
    }
  }
  return {smallestX, smallestY, largestX, largestY};
}

GS::Box Canvas::BBox(int shapeID) {
  for (const auto shape : shapeList) {
    if (shape->shapeID == shapeID) {
      Vec::Vec2D topLeft = shape->topLeftCoord();
      Vec::Vec2D bottomRight = shape->bottomRightCoord();
      return {topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
    }
  }
  POINT dimension = screenSize();
  return {0, 0, dimension.x, dimension.y};
}

bool Canvas::showConsole() {
  bool retVal = AllocConsole();
  AttachConsole(GetCurrentProcessId());
  // cppcheck-suppress leakReturnValNotUsed
  freopen("CONOUT$", "wb", stdout);
  // cppcheck-suppress leakReturnValNotUsed
  freopen("CONOUT$", "wb", stderr);
  return retVal;
}

bool Canvas::killConsole() {
  return FreeConsole();
}

int Canvas::addShape(GS::Shape *newShape) {
  std::shared_ptr<GS::Shape> newShape_(newShape);
  for (const auto &shape : shapeList) {
    if (GS::areEqual(shape, newShape_) &&
        (shape->shapeType != GS::TEXT)) {
      return -1;
    }
  }
  shapeList.push_back(newShape_);
  return newShape->shapeID;
}

// ~~~~~~~~~~~~~~~~~~~~~[ Tagging methods ]~~~~~~~~~~~~~~~~~~~~~~~~~~

bool Canvas::tagAbove(const std::string &tagName, int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID > shapeID) {
      shape->addTag(tagName);
      return true;
    }
  }
  return false;
}

bool Canvas::tagBelow(const std::string &tagName, int shapeID) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->shapeID < shapeID) {
      shape->addTag(tagName);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::tagAll(const std::string &tagName) {
  return tagAbove(tagName, -1);
}

bool Canvas::tagEnclosed(const std::string &tagName, GS::Box region) {
  return tagEnclosed(tagName, region.x1, region.y1, region.x2, region.y2);
}

bool Canvas::tagEnclosed(const std::string &tagName,
                         int x1,
                         int y1,
                         int x2,
                         int y2) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    POINT topLeft = {x1, y1};
    POINT bottomRight = {x2, y2};
    if (shape->shapeInRegion(topLeft, bottomRight)) {
      foundAny = true;
      shape->addTag(tagName);
    }
  }
  return foundAny;
}

bool Canvas::tagOverlapping(const std::string &tagName, GS::Box region) {
  return tagOverlapping(tagName, region.x1, region.y1, region.x2, region.y2);
}

bool Canvas::tagOverlapping(const std::string &tagName,
                            int x1,
                            int y1,
                            int x2,
                            int y2) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    POINT topLeft = {x1, y1};
    POINT bottomRight = {x2, y2};
    if (shape->overlapsWithRegion(topLeft, bottomRight)) {
      foundAny = true;
      shape->addTag(tagName);
    }
  }
  return foundAny;
}

bool Canvas::tagWithTag(const std::string &tagName, const std::string &newTag) {
  bool foundAny = true;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->addTag(newTag);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::tagWithTag(int shapeID, const std::string &newTag) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->addTag(newTag);
      return true;
    }
  }
  return false;
}

bool Canvas::tagClosest(const std::string &newTag, int x, int y) {
  float leastDistance = 1.0e6; // Dummy value
  std::shared_ptr<GS::Shape> closestShape(nullptr);
  Vec::Vec2D closestPoint;
  for (const auto &shape : shapeList) {
    closestPoint = shape->closestPointTo(x, y);
    float distance = closestPoint.magnitude(x, y);
    if (distance < leastDistance) {
      leastDistance = distance;
      closestShape = shape;
    }
  }
  if (closestShape) {
    closestShape->addTag(newTag);
    return true;
  }
  return false;
}

bool Canvas::deleteTag(int shapeID, const std::string &tagToDelete) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->removeTag(tagToDelete);
      foundAny = true;
    }
  }
  return foundAny;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool Canvas::penSize(const std::string &tagName, int width) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->penSize = width;
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::penSize(int shapeID, int width) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->penSize = width;
      return true;
    }
  }
  return false;
}

std::string Canvas::penColor(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      return shape->getPenColor();
    }
  }
  return "";
}

bool Canvas::penColor(const std::string &tagName, int red, int green, int blue) {
  return penColor(tagName, Colors::RGBTohex(red, green, blue));
}

bool Canvas::penColor(const std::string &tagName, std::string colorString) {
  colorString = Colors::hexValue(colorString);
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->setPenColor(colorString);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::penColor(int shapeID, int red, int green, int blue) {
  return penColor(shapeID, Colors::RGBTohex(red, green, blue));
}

bool Canvas::penColor(int shapeID, std::string colorString) {
  colorString = Colors::hexValue(colorString);
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->setPenColor(colorString);
      return true;
    }
  }
  return false;
}

std::string Canvas::getText(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      return shape->getText();
    }
  }
  return "";
}

void Canvas::setText(const std::string &tagName, const std::string &text) {
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->setText(text);
    }
  }
}

void Canvas::setText(int shapeID, const std::string &text) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->setText(text);
    }
  }
}

GS::FontAttr parseFont(std::string fontSpec) {
  GS::FontAttr prop;
  fontSpec = " " + fontSpec + " ";
  int bold = fontSpec.find(" bold ") != std::string::npos;
  if (bold) {
    prop.bold = FW_BOLD;
  }
  prop.underline = fontSpec.find(" underline ") != std::string::npos;
  prop.strikeout = fontSpec.find(" strikeout ") != std::string::npos;
  prop.italic = fontSpec.find(" italic ") != std::string::npos;
  return prop;
}

bool Canvas::setFont(int shapeID,
                     const std::string &fontStyle,
                     int size,
                     const std::string &fontFamily) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      GS::FontAttr prop = parseFont(fontStyle);
      prop.family = fontFamily;
      prop.size = size;
      shape->setFontAttr(prop);
      return true;
    }
  }
  return false;
}

bool Canvas::setFont(const std::string &tag,
                     const std::string &fontStyle,
                     int size,
                     const std::string &fontFamily) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tag)) {
      GS::FontAttr prop = parseFont(fontStyle);
      prop.family = fontFamily;
      prop.size = size;
      shape->setFontAttr(prop);
      foundAny = true;
    }
  }
  return foundAny;
}

GS::FontAttr Canvas::getFont(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->getFontAttr();
    }
  }
  return GS::FontAttr();
}

bool Canvas::borderStyle(int shapeID, GS::BorderStyle style) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->borderStyle(style);
      return true;
    }
  }
  return false;
}

GS::BorderStyle Canvas::borderStyle(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      return shape->borderStyle();
    }
  }
  return GS::INVALID_BORDER;
}

bool Canvas::borderStyle(const std::string &tag, GS::BorderStyle style) {
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tag)) {
      foundAny = true;
      shape->borderStyle(style);
    }
  }
  return foundAny;
}

std::string Canvas::fillColor(int shapeID) {
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      return shape->getFillColor();
    }
  }
  return "";
}

bool Canvas::fillColor(const std::string &tagName, int red, int green, int blue) {
  return fillColor(tagName, Colors::RGBTohex(red, green, blue));
}

bool Canvas::fillColor(const std::string &tagName, std::string colorStr) {
  colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());
  colorStr = Colors::hexValue(colorStr);
  bool foundAny = false;
  for (const auto &shape : shapeList) {
    if (shape->hasTag(tagName)) {
      shape->setFillColor(colorStr);
      foundAny = true;
    }
  }
  return foundAny;
}

bool Canvas::fillColor(int shapeID, int red, int green, int blue) {
  return fillColor(shapeID, Colors::RGBTohex(red, green, blue));
}

bool Canvas::fillColor(int shapeID, std::string colorString) {
  colorString = Colors::hexValue(colorString);
  for (const auto &shape : shapeList) {
    if (shape->shapeID == shapeID) {
      shape->setFillColor(colorString);
      return true;
    }
  }
  return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int Mouse::delta() {
  return wheelDelta;
}

int Mouse::xRoot() {
  GetCursorPos(&cursorCoord);
  return cursorCoord.x;
}

int Mouse::yRoot() {
  GetCursorPos(&cursorCoord);
  return cursorCoord.y;
}

int Mouse::y() {
  GetCursorPos(&cursorCoord);
  ScreenToClient(winHandle, &cursorCoord);
  return cursorCoord.y;
}

int Mouse::x() {
  GetCursorPos(&cursorCoord);
  ScreenToClient(winHandle, &cursorCoord);
  return cursorCoord.x;
}

bool Canvas::removeShape(int shapeID) {
  bool foundAny = false;
  auto hasID = [&](const std::shared_ptr<GS::Shape> &shape) {
    if (shape->shapeID == shapeID) {
      foundAny = true;
      return true;
    }
    return false;
  };
  auto lastShape = std::remove_if(shapeList.begin(), shapeList.end(), hasID);
  shapeList.erase(lastShape, shapeList.end());
  return foundAny;
}

bool Canvas::removeShape(const std::string &tagName) {
  bool foundAny = false;
  auto hasTag = [&](const std::shared_ptr<GS::Shape> &shape) {
    if (shape->hasTag(tagName)) {
      foundAny = true;
      return true;
    }
    return false;
  };
  auto lastShape = std::remove_if(shapeList.begin(), shapeList.end(), hasTag);
  shapeList.erase(lastShape, shapeList.end());
  return foundAny;
}

int Canvas::rectangle(GS::Box box) {
  return rectangle(box.x1, box.y1, box.x2, box.y2);
}

int Canvas::rectangle(int x1, int y1, int x2, int y2) {
  fixBBoxCoord(&x1, &y1, &x2, &y2);
  GS::Rect *rect(new GS::Rect(x1, y1, x2, y2));
  return addShape(rect);
}

int Canvas::oval(GS::Box box) {
  return oval(box.x1, box.y1, box.x2, box.y2);
}

int Canvas::oval(int x1, int y1, int x2, int y2) {
  fixBBoxCoord(&x1, &y1, &x2, &y2);
  GS::Oval *ellipse(new GS::Oval(x1, y1, x2, y2));
  return addShape(ellipse);
}

int Canvas::circle(POINT point, int radius) {
  return circle(point.x, point.y, radius);
}

int Canvas::circle(int x, int y, int radius) {
  GS::Circle *ellipse(new GS::Circle(x, y, radius));
  return addShape(ellipse);
}

int Canvas::text(POINT point, const std::string &txtStr, int width) {
  return text(point.x, point.y, txtStr, width);
}

int Canvas::text(int x, int y, const std::string &txtStr, int width) {
  GS::Text *txt(new GS::Text(x, y, txtStr, width));
  return addShape(txt);
}

int Canvas::arc(GS::Box box, GS::ArcType arcType, float pieSize, float tiltAngle) {
  return arc(box.x1, box.y1, box.x2, box.y2, arcType, pieSize, tiltAngle);
}

int Canvas::arc(int x1,
                int y1,
                int x2,
                int y2,
                GS::ArcType arcType,
                float pieSize,
                float tiltAngle) {
  fixBBoxCoord(&x1, &y1, &x2, &y2);
  pieSize = (pieSize < 0.0f) ? 360.0f + pieSize : pieSize;
  tiltAngle = (tiltAngle < 0.0f) ? 360.0f + tiltAngle : tiltAngle;
  while (pieSize > 360.0f) {
    pieSize -= 360.0f;
  }
  while (tiltAngle > 360.0f) {
    tiltAngle -= 360.0f;
  }
  GS::LineArc *lineArc(
    new GS::LineArc(x1, y1, x2, y2, arcType, pieSize, tiltAngle));
  return addShape(lineArc);
}

int Canvas::line(const std::vector<POINT> &lineCoords) {
  GS::Line *ligne(new GS::Line(lineCoords));
  return addShape(ligne);
}

int Canvas::polygon(const std::vector<POINT> &polyCoords) {
  GS::Poly *poly(new GS::Poly(polyCoords));
  return addShape(poly);
}

int Canvas::init(HINSTANCE hInstance, int cmdShow_) {
  // The resource object file must be linked with the program for the icon to show.
  windowClassEx.cbSize        = sizeof(WNDCLASSEX);
  windowClassEx.cbWndExtra    = sizeof(Canvas);
  windowClassEx.style         = classStyle;
  windowClassEx.lpfnWndProc   = Canvas::windowProcedure;
  windowClassEx.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(PROJECT_LOGO));
  windowClassEx.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(PROJECT_LOGO));
  windowClassEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
  windowClassEx.cbClsExtra    = 0;
  windowClassEx.hInstance     = hInstance;
  windowClassEx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_HIGHLIGHTTEXT);
  windowClassEx.lpszMenuName  = NULL;
  windowClassEx.lpszClassName = "Canvas";

  cmdShow = cmdShow_;
  winInst = hInstance;
  if (!RegisterClassEx(&windowClassEx)) {
    MessageBox(NULL, "Could not create window.", "Error", 0);
    return 0;
  }

  winHandle = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "Canvas",
                windowTitle.c_str(),
                windowStyle,
                xPos,
                yPos,
                winWidth,
                winHeight,
                NULL,
                NULL,
                hInstance,
                this // Save the pointer in lpParam for later
              );

  if (winHandle == NULL) {
    MessageBox(NULL, "Window Creation Failed!", "Error!",
               MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }

  return 1;
}

int Canvas::loop() {
  ShowWindow(winHandle, cmdShow);
  while (GetMessage(&windowMessage, NULL, 0, 0) > 0) {
    TranslateMessage(&windowMessage);
    DispatchMessage(&windowMessage);
  }
  killConsole();
  return static_cast<int>(windowMessage.wParam);
}

LRESULT PASCAL Canvas::windowProcedure(HWND winHandle,
                                       unsigned windowMessage,
                                       WPARAM wParam,
                                       LPARAM lParam) {
  // Get the object associated with the window
  Canvas *instance = reinterpret_cast<Canvas *>(GetWindowLongPtr(winHandle, 0));
  if (instance == 0) {
    if (windowMessage == WM_NCCREATE) {
      // Retrieve the objects address from the struct and associate that address
      // with the window. The pointer will be used in invoking class methods
      // from within this static window procedure.
      LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
      instance = reinterpret_cast<Canvas *>(createStruct->lpCreateParams);
      SetWindowLongPtr(winHandle, 0, reinterpret_cast<LONG_PTR>(instance));
      return instance->handleMessage(winHandle, windowMessage, wParam, lParam);
    } else {
      return DefWindowProc(winHandle, windowMessage, wParam, lParam);
    }
  } else {
    return instance->handleMessage(winHandle, windowMessage, wParam, lParam);
  }
}

LRESULT CALLBACK Canvas::handleMessage(const HWND &winHandle,
                                       unsigned windowMessage,
                                       WPARAM wParam,
                                       LPARAM lParam) {
  switch (windowMessage) {
    case WM_CREATE:
      break;
    case WM_PAINT: {
      PAINTSTRUCT paintStruct;
      HDC paintDC = BeginPaint(winHandle, &paintStruct);
      for (const auto &shape : shapeList) {
        HPEN oldPen, newPen;
        HBRUSH oldBrush, newBrush;
        COLORREF penColor = Colors::hexToColorRef(shape->getPenColor());
        newPen = CreatePen(shape->borderStyle(), shape->penSize, penColor);
        oldPen = static_cast<HPEN>(SelectObject(paintDC, newPen));
        std::string fillColor_ = shape->getFillColor();
        if (fillColor_ == "") {
          // Don't fill the shape
          newBrush = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
        } else {
          COLORREF fillColor = Colors::hexToColorRef(fillColor_);
          newBrush = CreateSolidBrush(fillColor);
        }
        oldBrush = static_cast<HBRUSH>(SelectObject(paintDC, newBrush));
        shape->draw(paintDC); // Draw the shape/object
        SelectObject(paintDC, oldBrush);
        DeleteObject(newBrush);
        SelectObject(paintDC, oldPen);
        DeleteObject(newPen);
      }
      EndPaint(winHandle, &paintStruct);
    }
    break;
    case WM_TIMER: {
      // wParam in this case is the timer ID
      callHandlers(TIMER, wParam);
      KillTimer(winHandle, wParam);
      InvalidateRect(winHandle, NULL, TRUE);
    }
    break;
    case WM_MOUSEMOVE: {
      trackMouse();
    }
    break;
    case WM_MOUSEHOVER: {
      callHandlers(HOVER);
    }
    break;
    case WM_MBUTTONDOWN: {
      if (callHandlers(WHEEL_CLICK)) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
    }
    break;
    case WM_LBUTTONDOWN: {
      bool called = false;
      if (ctrlKeyDown()) {
        called |= callHandlers(CTRL_LEFT_CLICK);
      } else if (altKeyDown()) {
        called |= callHandlers(ALT_LEFT_CLICK);
      } else {
        called |= callHandlers(LEFT_CLICK);
      }
      if (called) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
    }
    break;
    case WM_RBUTTONDOWN: {
      if (callHandlers(RIGHT_CLICK)) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
    }
    break;
    case WM_MOUSEWHEEL: {
      int wheelDelta = static_cast<short>(HIWORD(wParam));
      if (callHandlers(WHEEL_ROLL, wheelDelta / WHEEL_DELTA)) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
      return 0;
    }
    break;
    case WM_SYSCOMMAND: {
      // Disable normal ALT-key behaviour, and that irritating beep.
      // http://stackoverflow.com/questions/9627379
      if ((wParam == SC_KEYMENU) && ((lParam >> 16) <= 0)) {
        return 0;
      }
      return DefWindowProc(winHandle, windowMessage, wParam, lParam);
    }
    break;
    case WM_SYSKEYDOWN: {
      bool called = false;
      called |= callHandlers(ALT_KEY, wParam);
      if (shiftKeyDown()) {
        called |= callHandlers(ALT_SHIFT_KEY, wParam);
      }
      if (called) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
    }
    break;
    case WM_KEYDOWN: {
      bool called = false;
      called |= callHandlers(BARE_KEY, wParam);
      if (shiftKeyDown() && ctrlKeyDown()) {
        called |= callHandlers(CTRL_SHIFT_KEY, wParam);
      } else if (ctrlKeyDown()) {
        called |= callHandlers(CTRL_KEY, wParam);
      }
      if (called) {
        InvalidateRect(winHandle, NULL, TRUE);
      }
    }
    break;
    case WM_CLOSE:
      DestroyWindow(winHandle);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(winHandle, windowMessage, wParam, lParam);
  }
  return 0;
}

int Canvas::init() {
  return init(winInst, cmdShow);
}
