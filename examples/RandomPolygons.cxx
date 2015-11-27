#include "Canvas.h"

struct Handler : GC::EventHandler {
  GC::Canvas *canv = nullptr;
  int poly;
  Handler(GC::Canvas *canv, int poly) : canv(canv), poly(poly) {}
  virtual void handle(GC::Mouse) override {
    std::vector<int> nums(16);
    std::vector<POINT> points(8);
    std::generate(nums.begin(), nums.end(), std::rand);
    for (int i = 0; i < 8; i++) {
      int x = nums[i] % 900;
      int y = nums[i + 1] % 900;
      points[i] = {x, y};
    }
    canv->coords(poly, points);
  }
};

int main() {
  GC::Canvas canv;
  canv.init();
  int poly = canv.polygon({{10, 20}, {50, 60}, {100, 200}, {300, 250}});
  canv.fillColor(poly, "tan");
  canv.bind("<Mouse-1>", Handler(&canv, poly));
  return canv.loop();
}
