#include "Canvas.h"

struct Handler : GC::EventHandler {
  POINT lastPosition = { -100, -100};
  GC::Canvas *canv;
  explicit Handler(GC::Canvas *canv) : canv(canv) {}

  virtual void handle(GC::Mouse mouse) override {
    int x = mouse.x();
    int y = mouse.y();
    if ((lastPosition.x == -100) && (lastPosition.y == -100)) {
      lastPosition = {x, y};
      return;
    }
    canv->line({lastPosition, {x, y}});
    int node = canv->circle(x, y, 3);
    canv->fillColor(node, "tan");
    lastPosition = {x, y};
  }
};

int main() {
  GC::Canvas canv;
  canv.init();
  canv.text(10, 10, "Click around!!");
  canv.bind("<Mouse-1>", Handler(&canv));
  return canv.loop();
}
