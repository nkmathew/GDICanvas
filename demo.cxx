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
    canv->rectangle(canv->BBox(shape));
    if (mouse.delta() >= 0) {
      canv->moveShape(shape, x, y);
    } else  {
      canv->moveShape(shape, -x, -y);
    }
    canv->timer(1000, *this);
  }
};

int main() {
  Canvas canv;
  canv.init();
  int turtle;
  canv.showConsole();
  canv.icon("src/logo.ico");
  canv.text(420, 130, "all she wrote");
  canv.text(100, 130, "all she wrote");
  int text = canv.text(250, 130, "all she wrote");
  canv.penColor("text", "white");
  canv.fillColor("text", "forest green");
  canv.setFont("text", "bold");
  turtle = canv.polygon({{0, 16}, {-2, 14}, {-1, 10}, {-4, 7}, {-7, 9}, {-9, 8},
    {-6, 5}, {-7, 1}, {-5, -3}, {-8, -6}, {-6, -8}, {-4, -5}, {0, -7}, {4, -5},
    {6, -8}, {8, -6}, {5, -3}, {7, 1}, {6, 5}, {9, 8}, {7, 9}, {4, 7}, {1, 10},
    {2, 14}
  });
  canv.rectangle(canv.BBox(turtle));
  canv.fillColor(turtle, "yellow");
  canv.timer(1000, MoveTurtle(20, 30, text, &canv));
  canv.bind("<Mouse-1>", MoveTurtle(20, 30, text, &canv), "text");
  return canv.loop();
}
