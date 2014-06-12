#include <Canvas.h>

GC::Canvas canv(450, 450, "Hover test");

void complain(GCanvas::Mouse) {
  MessageBox(0, "Stop staring!", "Boxy box box", 32);
}

int main() {
  canv.init();
  int pie = canv.arc(10, 10, 400, 400, GS::PIE);
  canv.fillColor(pie, "Turquoise");
  canv.bind("<hover>", complain, "all");
  return canv.loop();
}

