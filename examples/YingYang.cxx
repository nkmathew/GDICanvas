#include "Canvas.h"

int main() {
  GC::Canvas canv(650, 650, "Ying Yang Demo");
  canv.init();
  int radius = 200;
  long yPos = canv.windowSize().y / 2;
  long xPos = canv.windowSize().x / 2;
  POINT centerTop = {xPos, yPos - radius / 2};
  POINT centerBottom = {xPos, yPos + radius / 2};
  std::string yinColor = "black";
  std::string yangColor = "white";
  GS::Box bigBox = {
    xPos - radius,
    yPos - radius,
    xPos + radius,
    yPos + radius,
  };
  radius /= 2;
  yPos -= radius;
  int bigLeftHalf = canv.arc(bigBox, GS::PIE, 180, 90);
  int bigRightHalf = canv.arc(bigBox, GS::PIE, 180, 270);

  int smallLeftHalf = canv.arc(
                        xPos - radius,
                        yPos - radius,
                        xPos + radius,
                        yPos + radius,
                        GS::CHORD, 180, 270
                      );
  yPos += radius * 2;
  int smallRightHalf = canv.arc(
                         xPos - radius,
                         yPos - radius,
                         xPos + radius,
                         yPos + radius,
                         GS::CHORD, 180, 90
                       );

  int smallTopCircle = canv.circle(centerTop, 25);
  int smallBottomCircle = canv.circle(centerBottom, 25);

  // Make all shapes borderless
  canv.borderStyle(bigLeftHalf, GS::NONE);
  canv.borderStyle(bigRightHalf, GS::NONE);
  canv.borderStyle(smallRightHalf, GS::NONE);
  canv.borderStyle(smallLeftHalf, GS::NONE);
  canv.borderStyle(smallBottomCircle, GS::NONE);
  canv.borderStyle(smallTopCircle, GS::NONE);

  // Fill them with alternating colors
  canv.fillColor(bigLeftHalf, yinColor);
  canv.fillColor(bigRightHalf, yangColor);
  canv.fillColor(smallLeftHalf, yinColor);
  canv.fillColor(smallRightHalf, yangColor);
  canv.fillColor(smallTopCircle, yangColor);
  canv.fillColor(smallBottomCircle, yinColor);

  return canv.loop();
}

