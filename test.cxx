#include <Canvas.h>

using namespace GCanvas;
using namespace GShape;

Canvas canv;
int turtle;
int oval;
void circle(int x, int y) {
  canv.fillColor(canv.circle(x, y, 5), "CHARTREUSE");
}

void mousePosition(Mouse mouse) {
  canv.coords(oval, {{145, 460}, {30, 140}, {340, 100}, {400, 120}, {523, 432}});
  canv.background("magenta");
  char text[10];
  snprintf(text, 10, "(%d, %d)", mouse.x(), mouse.y());
  canv.text(mouse.x(), mouse.y(), text);
}

void funky(Mouse mouse) {
  if (mouse.delta() > 0) {
    canv.moveShape(turtle, 20, 30);
  } else  {
    canv.moveShape(turtle, -20, -30);
  }
  circle(mouse.x(), mouse.y());
}

int main() {
  canv.init();
  canv.showConsole();
  oval = canv.circle(200, 300, 100);
  canv.fillColor(oval, "tan");
  canv.icon("src/logo.ico");
  int poly = canv.polygon({{5, 120}, {130, 140}, {340, 100}, {400, 120}, {523, 432}});
  canv.fillColor(poly, "purple");
  canv.raiseShape(oval, poly);
  turtle = canv.polygon({{0, 16}, { -2, 14}, { -1, 10}, { -4, 7}, { -7, 9}, { -9, 8},
    { -6, 5}, { -7, 1}, { -5, -3}, { -8, -6}, { -6, -8}, { -4, -5}, {0, -7}, {4, -5},
    {6, -8}, {8, -6}, {5, -3}, {7, 1}, {6, 5}, {9, 8}, {7, 9}, {4, 7}, {1, 10}, {2, 14}
  });

  canv.fillColor(turtle, "black");
  canv.bind("<wheel-roll>", funky);
  canv.bind("<Mouse-1>", mousePosition);
  return canv.loop();
}
