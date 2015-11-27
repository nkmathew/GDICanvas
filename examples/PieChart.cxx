#include "Canvas.h"

GC::Canvas canv(700, 700, "Pie Chart Demo");

std::map<int, int> calcDegrees(const std::vector<int> &values) {
  std::map<int, int> frequency;
  for (int val : values) {
    frequency[val] += 1;
  }
  return frequency;
}

int main() {
  canv.init();
  std::vector<int> values = {12, 13 , 12 , 3453 , 34534, 53, 4,
                             34 , 45 , 76, 544, 12, 121, 13, 13, 45
                            };

  std::map<int, int> freqs = calcDegrees(values);
  int nValues = values.size();
  GS::Box box = {100L, 100L, 600L, 600L};
  int angle = 0;
  int red = 10,
      green = 40,
      blue = 50;
  for (auto freqStruct : freqs) {
    float occurrences = freqStruct.second;
    int degree = (occurrences / nValues) * 360.0;
    int arcID = canv.arc(box, GS::PIE, degree, angle);
    angle += degree;
    canv.fillColor(arcID, red, green, blue);
    green = (green + 20) % 360;
    red = (red + 50) % 360;
    blue = (blue + 20) % 360;
  }
  return canv.loop();
}

