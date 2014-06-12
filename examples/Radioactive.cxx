#include "Canvas.h"

int main() {
  GC::Canvas canv;
  canv.init();
  int side = 400;
  int startX = 100;
  int startY = 100;
  int margin = 50;
  char outline[] = "black";
  char background[] = "yellow";
  int rect, first, second, third, circle;
  RECT box = {startX, startY, startX + side, startY + side};
  POINT center = {box.left + side / 2, box.top + side / 2};
  RECT bigCircle = {box.left + margin, box.top + margin, box.right - margin, box.bottom - margin};

  rect   = canv.rectangle(box);
  first  = canv.arc(bigCircle, GS::PIE, 60, 0);
  second = canv.arc(bigCircle, GS::PIE, 60, 120);
  third  = canv.arc(bigCircle, GS::PIE, 60, 240);
  circle = canv.circle(center.x, center.y, 30);

  canv.penSize(rect, 3);
  canv.penSize(circle, 4);

  canv.fillColor(rect, background);
  canv.fillColor(first, outline);
  canv.fillColor(second, outline);
  canv.fillColor(third, outline);
  canv.fillColor(circle, outline);

  canv.penColor(rect, outline);
  canv.penColor(first, outline);
  canv.penColor(second, outline);
  canv.penColor(third, outline);
  canv.penColor(circle, background);
  return canv.loop();
}

