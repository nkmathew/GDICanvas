#include "Canvas.h"

int main() {
  GC::Canvas canv(50, 50, 200, 200);
  canv.init();
  canv.title("Habari");
  canv.text(50, 50, "Habari dunia");
  return canv.loop();
}

