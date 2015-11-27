#include "Canvas.h"

GC::Canvas canv;

struct Handler : GC::EventHandler {
  int rolls = 0;
  void handle(GC::Mouse mouse) override {
    rolls += mouse.delta();
    rolls = (rolls < 0) ? 0 : rolls;
    rolls = (rolls > 10) ? 10 : rolls;
    if (mouse.delta() == -1) {
      canv.showShape(rolls);
    } else if (mouse.delta() == 1) {
      canv.hideShape(rolls);
    }
  }
};

int main() {
  canv.init();
  int width = 50;
  int length = 300;
  POINT start = {50, 50};
  int red = 20,
      green = 50,
      blue = 30;
  for (int i = 0; i < 10; i++) {
    POINT end = {start.x + length, start.y + width};
    int id = canv.rectangle(start.x, start.y, end.x, end.y);
    red = red + 30;
    green = green + 10;
    blue = blue + 70;
    canv.fillColor(id, red, green, blue);
    start.y += width;
  }
  canv.bind("<wheel-roll>", Handler());
  return canv.loop();
}
