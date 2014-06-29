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
  }
};

int main() {
  Canvas canv;
  canv.init();
  int turtle;
  canv.showConsole();
  canv.icon("src/logo.ico");
  int text = canv.text(100, 300, "Bazinga!!");
  canv.setFont(text, "bold strikeout underline", 30, "Consolas");
  FontAttr font = canv.getFont(text);

  turtle = canv.polygon({{0, 16}, { -2, 14}, { -1, 10}, { -4, 7}, { -7, 9}, { -9, 8},
    { -6, 5}, { -7, 1}, { -5, -3}, { -8, -6}, { -6, -8}, { -4, -5}, {0, -7}, {4, -5},
    {6, -8}, {8, -6}, {5, -3}, {7, 1}, {6, 5}, {9, 8}, {7, 9}, {4, 7}, {1, 10}, {2, 14}
  });
  canv.rectangle(canv.BBox(turtle));
  canv.fillColor(turtle, "yellow");
  canv.bind("<Mouse-1>", MoveTurtle(20, 30, turtle, &canv), text);
  return canv.loop();
}
