#include "Canvas.h"

GC::Canvas canv;

POINT lastPosition = { -100, -100};
void drawLine(GC::Mouse mouse) {
  int x = mouse.x();
  int y = mouse.y();
  if ((lastPosition.x == -100) && (lastPosition.y == -100)) {
    lastPosition = {x, y};
    return;
  }
  canv.line({lastPosition, {x, y}});
  int node = canv.circle(x, y, 3);
  canv.fillColor(node, "tan");
  lastPosition = {x, y};
}

int main() {
  canv.init();
  canv.text(10, 10, "Click around!!");
  canv.bind("<Mouse-1>", drawLine);
  return canv.loop();
}


