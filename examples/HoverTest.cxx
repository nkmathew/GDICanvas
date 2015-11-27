#include <Canvas.h>

struct Handler : GC::EventHandler {
  virtual void handle(GCanvas::Mouse mouse) override {
    char text[30];
    snprintf(text, 30, "Pointer at (%i, %i)", mouse.x(), mouse.y());
    MessageBox(0, text, "Mouse", 32);
  }
};

int main() {
  GC::Canvas canv(450, 450, "Hover test");
  canv.init();
  int pie = canv.arc(10, 10, 400, 400, GS::PIE);
  canv.fillColor(pie, "Turquoise");
  canv.bind("<hover>", Handler(), "all");
  return canv.loop();
}
