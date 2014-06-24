#include <Canvas.h>

using namespace GCanvas;
using namespace GShape;

struct MoveTurtle : GC::EventHandler {
  int x, y, shape;
  Canvas *canv;
  MoveTurtle(int x_, int y_, int id, Canvas *canv) {
    x = x_;
    y = y_;
    this->canv = canv;
    shape = id;
  }
  void handle(Mouse mouse) {
    if (mouse.delta() > 0) {
      canv->moveShape(shape, x, y);
    } else  {
      canv->moveShape(shape, -x, -y);
    }
  }
};

int main() {
  Canvas canv;
  canv.init();
  int turtle, oval, poly;
  canv.showConsole();
  oval = canv.circle(200, 300, 100);
  canv.fillColor(oval, "tan");
  canv.icon("src/logo.ico");
  poly = canv.polygon({{5, 120}, {130, 140}, {340, 100}, {400, 120}, {523, 432}});
  canv.fillColor(poly, "#333");
  canv.raiseShape(oval, poly);
  turtle = canv.polygon({{0, 16}, { -2, 14}, { -1, 10}, { -4, 7}, { -7, 9}, { -9, 8},
    { -6, 5}, { -7, 1}, { -5, -3}, { -8, -6}, { -6, -8}, { -4, -5}, {0, -7}, {4, -5},
    {6, -8}, {8, -6}, {5, -3}, {7, 1}, {6, 5}, {9, 8}, {7, 9}, {4, 7}, {1, 10}, {2, 14}
  });
  canv.fillColor(turtle, "yellow");
  canv.bind("<wheel-roll>", MoveTurtle(20, 30, turtle, &canv));
  return canv.loop();
}
