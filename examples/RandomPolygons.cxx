#include "Canvas.h"

GC::Canvas canv;
int poly = 0;

void reshapePolygon(GC::Mouse);

int main() {
  canv.init();
  canv.showConsole();
  poly = canv.polygon({{10, 20}, {50, 60}, {100, 200}, {300, 250}});
  canv.fillColor(poly, "tan");
  canv.bind("<Mouse-1>", reshapePolygon);
  return canv.loop();
}

void reshapePolygon(GC::Mouse) {
  std::vector<int> nums(16);
  std::vector<POINT> points(8);
  std::generate(nums.begin(), nums.end(), std::rand);
  for (int i = 0; i < 8; i++) {
    int x = nums[i] % 900;
    int y = nums[i + 1] % 900;
    points[i] = {x, y};
  }
  canv.coords(poly, points);
}

