/*!
 * \file Shapes.cxx
 */

#include "./Shapes.h"

using namespace GShape;

int Shape::counterID = 0;

// ~~~~~~~~~~~~~~~~~~~~~~~~~[ Free functions ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool GShape::areEqual(const std::shared_ptr<Shape> &first,
                      const std::shared_ptr<Shape> &second) {
  std::vector<POINT> vecFirst = first->coords();
  std::vector<POINT> vecSecond = second->coords();
  unsigned points = vecFirst.size();
  if (points != vecSecond.size()) {
    return false;
  }
  for (unsigned i = 0; i < points; i++) {
    if (Vec::Vec2D(vecFirst[i]) != Vec::Vec2D(vecSecond[i])) {
      return false;
    }
  }
  return true && (first->shapeType == second->shapeType);
}

bool GShape::pointInRegion(int xCoord,
                           int yCoord,
                           const Vec::Vec2D &topLeft,
                           const Vec::Vec2D &bottomRight) {
  if ((xCoord >= topLeft.x && yCoord >= topLeft.y) &&
      (xCoord <= bottomRight.x && yCoord <= bottomRight.y)) {
    return true;
  }
  return false;
}

bool GShape::pointInRegion(const Vec::Vec2D &point,
                           const Vec::Vec2D &topLeft,
                           const Vec::Vec2D &bottomRight) {
  return pointInRegion(point.x, point.y, topLeft, bottomRight);
}

bool GShape::regionsOverlap(const Vec::Vec2D &top1,
                            const Vec::Vec2D &bottom1,
                            const Vec::Vec2D &top2,
                            const Vec::Vec2D &bottom2) {
  return pointInRegion(top1, top2, bottom2) ||
         pointInRegion(top2, top2, bottom2) ||
         pointInRegion(top2, top1, bottom1) ||
         pointInRegion(top2, top1, bottom1);
}

Vec::Vec2D GShape::intersection(const Vec::Vec2D &start1,
                                const Vec::Vec2D &end1,
                                const Vec::Vec2D &start2,
                                const Vec::Vec2D &end2) {
  float grad1 = (end1.y - start1.y) / (end1.x - start1.x);
  float grad2 = (end2.y - start2.y) / (end2.x - start2.x);
  float inter1 = end1.y - (grad1 * end1.x);
  float inter2 = end2.y - (grad2 * end2.x);
  if ((grad1 == grad2) || (std::isinf(grad1) && std::isinf(grad2))) {
    // Parallel lines
    return {FLT_MAX, FLT_MAX};
  }
  float xIntersection, yIntersection;
  if (start1.x == end1.x) {
    // First line is perpendicular
    xIntersection = start1.x;
    yIntersection = grad2 * xIntersection + inter2;
  } else if (start2.x == end2.x) {
    // Second line is perpendicular
    xIntersection = start2.x;
    yIntersection = grad1 * xIntersection + inter1;
  } else {
    xIntersection = (inter1 - inter2) / (grad2 - grad1);
    yIntersection = grad1 * xIntersection + inter1;
  }
  return {xIntersection, yIntersection};
}

bool linesIntersect(const Vec::Vec2D &start1,
                    const Vec::Vec2D &end1,
                    const Vec::Vec2D &start2,
                    const Vec::Vec2D &end2) {
  Vec::Vec2D point = intersection(start1, end1, start2, end2);
  return (point.x == FLT_MAX) && (point.y == FLT_MAX);
}

bool GShape::withinLineSegment(const Vec::Vec2D &point,
                               const Vec::Vec2D &start,
                               const Vec::Vec2D &end) {
  return withinLineSegment(point.x, point.y, start, end);
}

bool GShape::withinLineSegment(float x,
                               float y,
                               const Vec::Vec2D &start,
                               const Vec::Vec2D &end) {
  float lineLength = Vec::Vec2D(start).magnitude(Vec::Vec2D(end));
  float distance = Vec::Vec2D(start).magnitude(x, y) +
                   Vec::Vec2D(end).magnitude(x, y);
  bool withinSegment = (distance - lineLength) < 1.0f;
  return withinSegment;
}

Vec::Vec2D GShape::bottomRightCoord(const std::vector<POINT> &coordList) {
  int points = coordList.size();
  if (points <= 2) {
    return {0, 0};
  }
  Vec::Vec2D first(coordList.front());
  float largestX = first.x;
  float largestY = first.y;
  for (int i = 0; i < points; i++) {
    Vec::Vec2D point(coordList[i]);
    largestX = (point.x > largestX) ? point.x : largestX;
    largestY = (point.y > largestY) ? point.y : largestY;
  }
  return {largestX, largestY};
}

Vec::Vec2D GShape::topLeftCoord(const std::vector<POINT> &coordList) {
  int points = coordList.size();
  if (points <= 2) {
    return {0, 0};
  }
  Vec::Vec2D first(coordList.front());
  float smallestX = first.x;
  float smallestY = first.y;
  for (int i = 0; i < points; i++) {
    Vec::Vec2D point(coordList[i]);
    smallestX = (point.x < smallestX) ? point.x : smallestX;
    smallestY = (point.y < smallestY) ? point.y : smallestY;
  }
  return {smallestX, smallestY};
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~[ Shape Base class ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*!
 * We assume the shame is defined in terms of its bounding box so that we don't
 * have to copy it for Rect, Oval and Text
 *
 * The coordinates will be assumed to be in the form [x1, y1, x2, y2] where `x1`
 * and `y1` are the top left coordinates and `x2` and `y2` the bottom left
 * coordinates
 */
void Shape::changeCoords(const std::vector<POINT> &coords) {
  topLeft = Vec::Vec2D(coords[0]);
  bottomRight = Vec::Vec2D(coords[1]);
}

BorderStyle Shape::borderStyle() {
  return border;
}

void Shape::borderStyle(BorderStyle style) {
  border = style;
}

std::vector<std::string> Shape::tags() {
  return tagList;
}

bool Shape::addTag(const std::string &newTag) {
  for (const std::string &tag : tagList) {
    if (newTag == tag) {
      return false;
    }
  }
  tagList.push_back(newTag);
  return true;
}

void Shape::removeTag(const std::string &tag) {
  if (tag == "all") {
    // Make sure there's at least one tag in the vector.
    return;
  }
  auto iterPos = std::remove(tagList.begin(), tagList.end(), tag);
  tagList.erase(iterPos);
}

ShapeType Shape::type() {
  return shapeType;
}

void Shape::setFillColor(std::string fillColor_) {
  fillColor = fillColor_;
}

void Shape::setPenColor(std::string penColor_) {
  penColor = (penColor_ != "") ? penColor_ : penColor;
}

std::string Shape::getFillColor() {
  return fillColor;
}

std::string Shape::getPenColor() {
  return penColor;
}

bool Shape::hasTag(std::string tagName) {
  for (const std::string &tag : tagList) {
    if (tag == tagName) {
      return true;
    }
  }
  return false;
}

Vec::Vec2D Shape::closestPointTo(int x, int y) {
  Vec::Vec2D bottomRight = bottomRightCoord();
  Vec::Vec2D topLeft = topLeftCoord();
  Vec::Vec2D vector = Vec::Vec2D(bottomRight) - Vec::Vec2D(topLeft);
  Vec::Vec2D corners[] = {
    Vec::Vec2D(topLeft),
    Vec::Vec2D(topLeft) + vector,
    Vec::Vec2D(bottomRight),
    Vec::Vec2D(bottomRight) - vector
  };
  Vec::Vec2D closestPoint(corners[0]);
  float distance = corners[0].magnitude(x, y);
  float leastDistance = distance;
  for (int i = 0; i < 4; i++) {
    float distance = corners[i].magnitude(x, y);
    if (distance > leastDistance) {
      leastDistance = distance;
      closestPoint = corners[i];
    }
  }
  return closestPoint;
}

int Shape::BBoxHeight() {
  Vec::Vec2D bottomRight_ = bottomRightCoord();
  Vec::Vec2D topLeft_ = topLeftCoord();
  return abs(bottomRight_.y - topLeft_.y) / 2;
}

int Shape::BBoxLength() {
  Vec::Vec2D bottomRight_ = bottomRightCoord();
  Vec::Vec2D topLeft_ = topLeftCoord();
  return abs(bottomRight_.x - topLeft_.x) / 2;
}

bool Shape::pointInEllipse(int x, int y) {
  Vec::Vec2D center = BBoxCenter();
  float a = pow((x - center.x), 2.0f) / pow(BBoxLength(), 2.0f);
  float b = pow((y - center.y), 2.0f) / pow(BBoxHeight(), 2.0f);
  return (a + b) <= 1.0f;
}

Vec::Vec2D Shape::BBoxCenter() {
  return {(bottomRight.x + topLeft.x) / 2.0f,
          (bottomRight.y + topLeft.y) / 2.0f
         };
}

bool Shape::shapeInRegion(const Vec::Vec2D &topLeft_,
                          const Vec::Vec2D &bottomRight_) {
  Vec::Vec2D topLeft = topLeftCoord();
  Vec::Vec2D bottomRight = bottomRightCoord();
  return pointInRegion(topLeft, topLeft_, bottomRight_) &&
         pointInRegion(bottomRight, topLeft_, bottomRight_);
}

bool Shape::pointInShape(const Vec::Vec2D &point) {
  return pointInShape(point.x, point.y);
}

// Check if any of the four vertices of the region are inside the shape.
bool Shape::regionOverlapsShape(const Vec::Vec2D &topLeft_,
                                const Vec::Vec2D &bottomRight_) {
  Vec::Vec2D topRight = {bottomRight_.x, topLeft_.y};
  Vec::Vec2D bottomLeft = {topLeft_.x, bottomRight_.y};
  return pointInShape(topLeft_) || pointInShape(bottomRight_) ||
         pointInShape(topRight) || pointInShape(bottomLeft);
}

/*!
 * Ellipse overlapping scenarios:
 *    + The rectangular region has at least one of its vertices in the ellipse
 *    + The furthest points in the ellipse's cicumference are inside the region.
 */
bool Shape::ellipseOverlapsRegion(const Vec::Vec2D &topLeft_,
                                  const Vec::Vec2D &bottomRight_) {
  Vec::Vec2D center = BBoxCenter();
  bool regionOverlaps = regionOverlapsShape(topLeft_, bottomRight_);
  bool pointsInRegion =
    pointInRegion(bottomRight.x, center.y, topLeft_, bottomRight_) ||
    pointInRegion(center.x, bottomRight.y, topLeft_, bottomRight_) ||
    pointInRegion(topLeft.x, center.y, topLeft_, bottomRight_) ||
    pointInRegion(center.x, topLeft.y, topLeft_, bottomRight_);
  return  pointsInRegion || regionOverlaps;
}

bool Shape::BBoxOverlapsRegion(const Vec::Vec2D &top,
                               const Vec::Vec2D &bottom) {
  Vec::Vec2D bottomRight_ = bottomRightCoord();
  Vec::Vec2D topLeft_ = topLeftCoord();
  return regionsOverlap(topLeft_, bottomRight_, top, bottom);
}

/**
 * Uses the ellipse formula with `x` as the subject
 */
ELLIPSE_POINTS Shape::ordinateToCoord(float y) {
  float a = BBoxLength();
  float b = BBoxHeight();
  Vec::Vec2D center = BBoxCenter();
  y -= center.y;
  float x = sqrt((a * a) * (1.0f - ((y * y) / (b * b))));
  y += center.y;
  if (std::isnan(x)) {
    // No valid point found
    return {
      {FLT_MAX, y},
      {-FLT_MAX, y}
    };
  }
  return {
    {center.x + x, y},
    {center.x - x, y}
  };
}

/**
 * Uses the ellipse formula with `y` as the subject
 */
ELLIPSE_POINTS Shape::abscissaToCoord(float x) {
  float a = BBoxLength();
  float b = BBoxHeight();
  Vec::Vec2D center = BBoxCenter();
  x -= center.x;
  float y = sqrt((b * b) * (1.0f - ((x * x) / (a * a))));
  x += center.x;
  if (std::isnan(y)) {
    // No valid point found
    return {
      {x, FLT_MAX},
      {x, -FLT_MAX}
    };
  }
  return {
    {x, center.y + y},
    {x, center.y - y}
  };
}

void Shape::move(int xAmount, int yAmount) {
  Vec::Vec2D vector(xAmount, yAmount);
  topLeft = topLeft + vector;
  bottomRight = bottomRight + vector;
}

std::vector<POINT> Shape::BBoxCoords()  const {
  POINT topLeft_ = static_cast<POINT>(topLeftCoord());
  POINT bottomRight_ = static_cast<POINT>(bottomRightCoord());
  return {
    {topLeft_.x, topLeft_.y},
    {bottomRight_.x, topLeft_.y},
    {bottomRight_.x, bottomRight_.y},
    {topLeft_.x, bottomRight_.y}
  };
}

// Shapes defined by their bounding box coordinates are handled here. The rest,
// like Polygons and lines will override the method.
std::vector<POINT> Shape::coords() const {
  return BBoxCoords();
}

bool Shape::isShown() {
  return isDrawn;
}

void Shape::visibility(bool visible) {
  isDrawn = visible;
}

void Shape::setFontAttr(const FontAttr &fontProp_) {
  fontProp = fontProp_;
}

FontAttr Shape::getFontAttr() {
  return fontProp;
}

std::string Shape::getText() {
  return text;
}

void Shape::setText(const std::string &text_) {
  text = text_;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~[ Polygon ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Poly::changeCoords(const std::vector<POINT> &coords) {
  polyCoords = coords;
}

void Poly::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  int vertices = polyCoords.size();
  for (int i = 0; i < vertices; i++) {
    coordinates[i] = polyCoords[i];
  }
  Polygon(paintDC, coordinates, vertices);
}

Vec::Vec2D Poly::topLeftCoord() const {
  return ::topLeftCoord(polyCoords);
}

Vec::Vec2D Poly::bottomRightCoord() const {
  return ::bottomRightCoord(polyCoords);
}

bool Poly::pointInShape(int x_, int y_) {
  // Adapted from Randolph Franklin's ray casting algorithm
  // http:///www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
  float x = x_;
  float y = y_;
  int index = 0;
  int vertices = polyCoords.size();
  while (index < vertices) {
    POINT point = polyCoords[index];
    xCoords[index] = point.x;
    yCoords[index] = point.y;
    index++;
  }
  int i, j, c = 0;
  for (i = 0, j = vertices - 1; i < vertices; j = i++) {
    if (((yCoords[i] > y) != (yCoords[j] > y)) &&
        (x < (xCoords[j] - xCoords[i]) * (y - yCoords[i]) /
         (yCoords[j] - yCoords[i]) + xCoords[i])) {
      c = !c;
    }
  }
  return c;
}

bool Poly::shapeInRegion(const Vec::Vec2D &topLeft,
                         const Vec::Vec2D &bottomRight) {
  for (const POINT &coord : polyCoords) {
    Vec::Vec2D vertex = coord;
    if (!pointInRegion(vertex.x, vertex.y, topLeft, bottomRight)) {
      return false;
    }
  }
  return true;
}

bool Poly::overlapsWithRegion(const Vec::Vec2D &topLeft,
                              const Vec::Vec2D &bottomRight) {
  // At least one point has to be within that region.
  for (const POINT &coord : polyCoords) {
    Vec::Vec2D vertex = coord;
    if (pointInRegion(vertex.x, vertex.y, topLeft, bottomRight)) {
      return true;
    }
  }
  return false || regionOverlapsShape(topLeft, bottomRight);
}

Vec::Vec2D Poly::closestPointTo(int x, int y) {
  // Returns the closest vertex to the specified point.
  POINT firstPoint = polyCoords[0];
  Vec::Vec2D closestPoint(firstPoint);
  float leastDistance = Vec::Vec2D(firstPoint).magnitude(x, y);
  for (const POINT &coord : polyCoords) {
    float distance = closestPoint.magnitude(x, y);
    if (distance < leastDistance) {
      leastDistance = distance;
      closestPoint = Vec::Vec2D(coord);
    }
  }
  return closestPoint;
}

std::vector<POINT> Poly::coords() const {
  std::vector<POINT> coordVector;
  int vertices = polyCoords.size();
  for (int i = 0; i < vertices; i++) {
    coordVector.push_back(polyCoords[i]);
  }
  return coordVector;
}

void Poly::move(int xAmount, int yAmount) {
  Vec::Vec2D vector(xAmount, yAmount);
  int vertices = polyCoords.size();
  for (int i = 0; i < vertices; i++) {
    POINT point = polyCoords[i];
    polyCoords[i] = Vec::Vec2D(point) + vector;
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~[ Rectangle ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Rect::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  Rectangle(paintDC, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

Vec::Vec2D Rect::topLeftCoord() const {
  return topLeft;
}

Vec::Vec2D Rect::bottomRightCoord() const {
  return bottomRight;
}

bool Rect::pointInShape(int x, int y) {
  if ((x >= topLeft.x && y >= topLeft.y) &&
      (x <= bottomRight.x && y <= bottomRight.y)) {
    return true;
  }
  return false;
}

bool Rect::overlapsWithRegion(const Vec::Vec2D &topLeft_,
                              const Vec::Vec2D &bottomRight_) {
  return BBoxOverlapsRegion(topLeft_, bottomRight_);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~[ Text ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Text::createFont(HFONT *font) {
  FontAttr fontProp = getFontAttr();
  HDC hDC = GetDC(NULL);
  LONG fontHeight = -MulDiv(fontProp.size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
  ReleaseDC(NULL, hDC);
  *font = CreateFont(fontHeight, 0, 0, 0, fontProp.bold, fontProp.italic,
                     fontProp.underline, fontProp.strikeout, DEFAULT_CHARSET,
                     OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                     VARIABLE_PITCH, fontProp.family.c_str());
}

POINT Text::textArea(HDC paintDC) {
  SIZE size;
  std::string text_ = getText();
  GetTextExtentPoint32(paintDC, text_.c_str(), text_.length(), &size);
  return {size.cx, size.cy};
}

void Text::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  HFONT font;
  createFont(&font);
  SelectObject(paintDC, font);
  POINT dim = textArea(paintDC);
  int x1 = static_cast<int>(start.x);
  int y1 = static_cast<int>(start.y);
  int x2 = x1 + dim.x;
  int y2 = y1 + dim.y;
  if (width != 0) {
    x2 = x1 + width;
  }

  // Update the bounding box's coordinates with the correct values according to
  // the current font.
  topLeft = {x1, y1};
  bottomRight = {x2, y2};

  RECT textRegion = {x1, y1, x2, y2};
  SetTextColor(paintDC, Colors::hexToColorRef(getPenColor()));
  std::string fillColor_ = getFillColor();
  if (fillColor_ != "") {
    SetBkColor(paintDC, Colors::hexToColorRef(fillColor_));
  } else {
    SetBkMode(paintDC, TRANSPARENT);
  }
  std::string text_ = getText();
  int format =  DT_NOCLIP | DT_SINGLELINE | DT_WORD_ELLIPSIS;
  DrawText(paintDC, text_.c_str(), -1, &textRegion, format);
  DeleteObject(font);
}

Vec::Vec2D Text::topLeftCoord() const {
  return topLeft;
}

Vec::Vec2D Text::bottomRightCoord() const {
  return bottomRight;
}

bool Text::pointInShape(int x, int y) {
  return pointInRegion(x, y, topLeft, bottomRight);
}

bool Text::overlapsWithRegion(const Vec::Vec2D &topLeft_,
                              const Vec::Vec2D &bottomRight_) {
  return BBoxOverlapsRegion(topLeft_, bottomRight_);
}

void Text::move(int xAmount, int yAmount) {
  Vec::Vec2D vector(xAmount, yAmount);
  start = start + vector;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~[ Oval ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Oval::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  Ellipse(paintDC, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

Vec::Vec2D Oval::topLeftCoord() const {
  return topLeft;
}

Vec::Vec2D Oval::bottomRightCoord() const {
  return bottomRight;
}

bool Oval::pointInShape(int x, int y) {
  return pointInEllipse(x, y);
}

bool Oval::overlapsWithRegion(const Vec::Vec2D &topLeft_,
                              const Vec::Vec2D &bottomRight_) {
  return ellipseOverlapsRegion(topLeft_, bottomRight_);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~[ Circle ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Circle::changeCoords(const std::vector<POINT> &coords) {
  center = coords[0];
  radius = coords[1].x;
}

void Circle::updateBBoxCoords() {
  topLeft = {center.x - radius, center.y - radius};
  bottomRight = {center.x + radius, center.y + radius};
}

void Circle::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  updateBBoxCoords();
  Ellipse(paintDC, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

Vec::Vec2D Circle::bottomRightCoord() const {
  Vec::Vec2D bottomRight = {center.x + radius, center.y + radius};
  return bottomRight;
}

Vec::Vec2D Circle::topLeftCoord() const {
  Vec::Vec2D topLeft = {center.x - radius, center.y - radius};
  return topLeft;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~[ Line ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Line::changeCoords(const std::vector<POINT> &coords) {
  lineCoords = coords;
}

void Line::move(int xAmount, int yAmount) {
  Vec::Vec2D vector(xAmount, yAmount);
  int points = lineCoords.size();
  for (int i = 0; i < points; i++) {
    POINT point = lineCoords[i];
    lineCoords[i] = Vec::Vec2D(point) + vector;
  }
}

void Line::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  POINT firstElem = lineCoords.front();
  MoveToEx(paintDC, firstElem.x, firstElem.y, NULL);
  for (const POINT &point : lineCoords) {
    LineTo(paintDC, point.x, point.y);
  }
}

bool Line::shapeInRegion(const Vec::Vec2D &topLeft, const Vec::Vec2D &bottomRight) {
  int points = lineCoords.size();
  for (int i = 0; i < points; i++) {
    Vec::Vec2D point = lineCoords[i];
    if (!pointInRegion(point.x, point.y, topLeft, bottomRight)) {
      return false;
    }
  }
  return true;
}

//! \see http://stackoverflow.com/questions/910882
bool Line::pointInLine(const Vec::Vec2D &start,
                       const Vec::Vec2D &end,
                       float x,
                       float y) {
  // Tells whether the point is within a certain distance of the line
  float lineLength = Vec::Vec2D(start).magnitude(Vec::Vec2D(end));
  bool withinSegment = withinLineSegment(x, y, start, end);
  if ((start.x == end.x) && (start.x == x)) {
    // Handle a click on a perpendicular line
    return true && withinSegment;
  } else if (start.y == end.y) {
    // Horizontal line
    return (std::abs(start.y - y) < 3.0f) && withinSegment;
  }
  float perpDistance = std::abs(((end.x - start.x) * (start.y - y)) -
                                ((start.x - x) * (end.y - start.y)));
  perpDistance /= lineLength;
  return (perpDistance < (1.0f + penSize)) && withinSegment;
}

bool Line::pointInShape(int x, int y) {
  int points = lineCoords.size();
  for (int i = 0; i < points - 1; i++) {
    Vec::Vec2D start = lineCoords[i];
    Vec::Vec2D end = lineCoords[i + 1];
    if (pointInLine(start, end, x, y)) {
      // Makes sure the point is between the start and end even if it
      // technically is on the line.
      return true;
    }
  }
  return false;
}

bool Line::overlapsWithRegion(const Vec::Vec2D &topLeft,
                              const Vec::Vec2D &bottomRight) {
  // The line overlaps with the region if it intersects with at least one side
  // of the rectangular region or is completely inside it.
  Vec::Vec2D topRight = {bottomRight.x, topLeft.y};
  Vec::Vec2D bottomLeft = {topLeft.x, bottomRight.y};
  int points = lineCoords.size();
  for (int i = 0; i < points - 1; i++) {
    Vec::Vec2D start = lineCoords[i];
    Vec::Vec2D end = lineCoords[i + 1];
    Vec::Vec2D top = intersection(start, end, topLeft, topRight);
    Vec::Vec2D bottom = intersection(start, end, bottomLeft, bottomRight);
    Vec::Vec2D left = intersection(start, end, bottomLeft, topLeft);
    Vec::Vec2D right = intersection(start, end, bottomRight, topRight);
    // Test if the intersection point is both in the region and in the line segment
    bool overlaps = (pointInLine(topLeft, topRight, top.x, top.y) &&
                     pointInLine(start, end, top.x, top.y)) ||
                    (pointInLine(bottomLeft, bottomRight, bottom.x, bottom.y) &&
                     pointInLine(start, end, bottom.x, bottom.y)) ||
                    (pointInLine(bottomLeft, topLeft, left.x, left.y) &&
                     pointInLine(start, end, left.x, left.y)) ||
                    (pointInLine(bottomRight, topRight, right.x, right.y) &&
                     pointInLine(start, end, right.x, right.y));

    if (overlaps) {
      return true;
    }
  }
  return false;
}

Vec::Vec2D Line::bottomRightCoord() const {
  return ::bottomRightCoord(lineCoords);
}

Vec::Vec2D Line::topLeftCoord() const {
  return ::topLeftCoord(lineCoords);
}

std::vector<POINT> Line::coords() const {
  std::vector<POINT> coordVector;
  int points = lineCoords.size();
  for (int i = 0; i < points; i++) {
    coordVector.push_back(lineCoords[i]);
  }
  return coordVector;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~[ Arc ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Vec::Vec2D LineArc::bottomRightCoord() const {
  return bottomRight;
}

Vec::Vec2D LineArc::topLeftCoord() const {
  return topLeft;
}

Vec::Vec2D LineArc::endPoint() const {
  Vec::Vec2D end = coordFromAngle(tiltAngle + pieSize);
  return {end.x, end.y};
}

Vec::Vec2D LineArc::startPoint() const {
  Vec::Vec2D start = coordFromAngle(tiltAngle);
  return {start.x, start.y};
}

/*!
 * Works by generating the possible intersection points between the ellipse of
 * which the arc is part of and the region using abscissaToCoord() and
 * ordinateToCoord().
 *
 * And then checks if at least one of the points is both on the arc and in that
 * region.
 */
bool LineArc::arcOverlapsRegion(const Vec::Vec2D &topLeft_,
                                const Vec::Vec2D &bottomRight_) {
  // The possible intersection points on the circumference
  ELLIPSE_POINTS p1 = ordinateToCoord(topLeft_.y);
  ELLIPSE_POINTS p2 = ordinateToCoord(bottomRight_.y);
  ELLIPSE_POINTS p3 = abscissaToCoord(topLeft_.x);
  ELLIPSE_POINTS p4 = abscissaToCoord(bottomRight_.x);
  return
    (pointOnArc(p1.first) && pointInRegion(p1.first, topLeft_, bottomRight_)) ||
    (pointOnArc(p1.second) && pointInRegion(p1.second, topLeft_, bottomRight_)) ||
    (pointOnArc(p2.first) && pointInRegion(p2.first, topLeft_, bottomRight_)) ||
    (pointOnArc(p2.second) && pointInRegion(p2.second, topLeft_, bottomRight_)) ||
    (pointOnArc(p3.first) && pointInRegion(p3.first, topLeft_, bottomRight_)) ||
    (pointOnArc(p3.second) && pointInRegion(p3.second, topLeft_, bottomRight_)) ||
    (pointOnArc(p4.first) && pointInRegion(p4.first, topLeft_, bottomRight_)) ||
    (pointOnArc(p4.second) && pointInRegion(p4.second, topLeft_, bottomRight_));
}

/*!
 * Works like arcOverlapsRegion() handling the extra case specific to chords
 * where one of the region's vertices goes through the chord's straight line
 */
bool LineArc::chordOverlapsRegion(const Vec::Vec2D &topLeft_,
                                  const Vec::Vec2D &bottomRight_) {
  bool overlapsArc = arcOverlapsRegion(topLeft_, bottomRight_);
  Vec::Vec2D start = startPoint();
  Vec::Vec2D end = endPoint();
  Vec::Vec2D p1 = {topLeft_.y, bottomRight_.x};
  Vec::Vec2D p2 = {topLeft_.x, bottomRight_.y};
  bool anyIntersect = linesIntersect(topLeft_, p1, start, end) ||
                      linesIntersect(topLeft_, p2, start, end) ||
                      linesIntersect(bottomRight_, p1, start, end) ||
                      linesIntersect(bottomRight_, p2, start, end);
  return overlapsArc || anyIntersect;
}

/*!
 * Works like arcOverlapsRegion() handling the extra case specific to pies
 * where one of the region's vertices goes through the pie's two straight lines
 */
bool LineArc::pieOverlapsRegion(const Vec::Vec2D &topLeft_,
                                const Vec::Vec2D &bottomRight_) {
  bool overlapsArc = arcOverlapsRegion(topLeft_, bottomRight_);
  Vec::Vec2D center = BBoxCenter();
  Vec::Vec2D start = startPoint();
  Vec::Vec2D end = endPoint();
  Vec::Vec2D p1 = {bottomRight_.x, topLeft_.y};
  Vec::Vec2D p2 = {topLeft_.x, bottomRight_.y};
  Vec::Vec2D intersections[] = {
    intersection(topLeft_, p1, start, center),
    intersection(topLeft_, p1, end, center),
    intersection(topLeft_, p2, start, center),
    intersection(topLeft_, p2, end, center),
    intersection(bottomRight_, p1, start, center),
    intersection(bottomRight_, p1, end, center),
    intersection(bottomRight_, p2, start, center),
    intersection(bottomRight_, p2, end, center)
  };
  bool anyIntersect = false;
  for (int i = 7; i >= 0; i--) {
    Vec::Vec2D point = intersections[i];
    // Check if the intersection lies on the pie's borders.
    bool onBorderLines = withinLineSegment(point, center, start) ||
                         withinLineSegment(point, center, end);
    anyIntersect |= (pointInRegion(point, topLeft_, bottomRight_) &&
                     onBorderLines);
  }
  return overlapsArc || anyIntersect;
}

bool LineArc::overlapsWithRegion(const Vec::Vec2D &topLeft_,
                                 const Vec::Vec2D &bottomRight_) {
  switch (arcType) {
    case PIE:
      return pieOverlapsRegion(topLeft_, bottomRight_);
    case CHORD:
      return chordOverlapsRegion(topLeft_, bottomRight_);
    case ARC:
      return arcOverlapsRegion(topLeft_, bottomRight_);
  }
  return false;
}

float LineArc::sign(const Vec::Vec2D &p1,
                    const Vec::Vec2D &p2,
                    const Vec::Vec2D &p3) {
  return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool LineArc::pointInTriangle(const Vec::Vec2D &point,
                              const Vec::Vec2D &v1,
                              const Vec::Vec2D &v2,
                              const Vec::Vec2D &v3) {
  bool b1, b2, b3;
  b1 = sign(point, v1, v2) < 0.0f;
  b2 = sign(point, v2, v3) < 0.0f;
  b3 = sign(point, v3, v1) < 0.0f;
  return ((b1 == b2) && (b2 == b3));
}

bool LineArc::pointOnArc(const Vec::Vec2D &point) {
  return pointOnArc(point.x, point.y);
}

bool LineArc::pointOnArc(int x, int y) {
  Vec::Vec2D center = BBoxCenter();
  float a = pow((x - center.x), 2.0f) / pow(BBoxLength(), 2.0f);
  float b = pow((y - center.y), 2.0f) / pow(BBoxHeight(), 2.0f);
  float angle = angleFromCoord(x, y);
  float endAngle = pieSize + tiltAngle;
  if (endAngle <= 360.0f) {
    bool withinSector = (angle >= tiltAngle) && (angle <= endAngle);
    return withinSector && (std::abs(a + b - 1.0f) < 0.02f);
  } else {
    // Handle the case where the arc lies on the right center line, one portion
    // above and another below.
    endAngle = (endAngle > 360.0f) ? endAngle - 360.0f : endAngle;
    bool withinSector = (angle >= tiltAngle) || (angle <= endAngle);
    return withinSector && (std::abs(a + b - 1.0f) < 0.02f);
  }
}

bool LineArc::pointInPie(int x, int y) {
  float angle = angleFromCoord(x, y);
  float endAngle = pieSize + tiltAngle;
  if (endAngle <= 360.0f) {
    bool withinSector = (angle >= tiltAngle) && (angle <= endAngle);
    return withinSector && pointInEllipse(x, y);
  } else {
    // Handle the case where the pie lies on the right center line, one portion
    // above and another below.
    endAngle = (endAngle > 360.0f) ? endAngle - 360.0f : endAngle;
    bool withinSector = (angle >= tiltAngle) || (angle <= endAngle);
    return withinSector && pointInEllipse(x, y);
  }
}

bool LineArc::pointInChord(int x, int y) {
  Vec::Vec2D point = Vec::Vec2D(x, y);
  Vec::Vec2D center = Vec::Vec2D(BBoxCenter());
  Vec::Vec2D tiltPos = coordFromAngle(tiltAngle);
  Vec::Vec2D endPos = coordFromAngle(tiltAngle + pieSize);
  bool inTriangle = pointInTriangle(point, center, tiltPos, endPos);
  if (pieSize > 180.0f) {
    return pointInPie(x, y) || inTriangle;
  } else {
    return pointInPie(x, y) && !inTriangle;
  }
}

bool LineArc::pointInShape(const Vec::Vec2D &point) {
  return pointInShape(point.x, point.y);
}

bool LineArc::pointInShape(int x, int y) {
  switch (arcType) {
    case PIE:
      return pointInPie(x, y);
    case CHORD:
      return pointInChord(x, y);
    case ARC:
      return pointOnArc(x, y);
  }
  return false;
}

float LineArc::angleFromCoord(float x, float y) {
  // Returns the anticlockwise angle between (x, y) and the center line
  Vec::Vec2D center = BBoxCenter();
  float gradient = (center.y - y) / (center.x - x);
  float angleBetween = atan(gradient) * 180.0f / PI;
  if (x == center.x) {
    return (y < center.y) ? 90.0f : 270.0f;
  } else if (center.y == y) {
    return (x < center.x) ? 180.0f : 0.0f;
  }
  // Return the angle according to the sector
  if (angleBetween > 0.0f) {
    return (y < center.y) ? 180.0f - angleBetween : 360.0f - angleBetween;
  } else {
    return (y < center.y) ? -angleBetween : 180.0f - angleBetween;
  }
}

Vec::Vec2D LineArc::coordFromAngle(float angle) const {
  float r1 = abs(bottomRight.x - topLeft.x) / 2;
  float r2 = abs(bottomRight.y - topLeft.y) / 2;
  Vec::Vec2D center(topLeft.x + r1, topLeft.y + r2);
  while (angle > 360.0f) {
    angle -= 360.0f;
  }
  if ((angle != 90.0f) && (angle != 270.0f)) {
    // Make the given angle be the size of the PIE, not the chunked out part.
    // XXX: This if-test prevents 270 from being drawn like 90
    angle = 360.0f - angle;
  }
  float radians = angle * PI / 180.0f;
  // Use ellipse formula to find the ordinate and abscissa given an angle.
  float x = (r1 * r2) / (sqrt((r2 * r2) + (r1 * r1 * pow(tan(radians), 2.0f))));
  float y = x * tan(radians);
  if (((angle > 90.0f) && (angle <= 180.0f)) ||
      ((angle >= 180.0f) && (angle <= 270.0f))) {
    // Makes sure angles with the same tangents are drawn correctly.
    x = -x;
    y = -y;
  }
  return Vec::Vec2D(x, y) + center;
}

void LineArc::draw(HDC paintDC) {
  if (!isShown()) {
    return;
  }
  int x1 = topLeft.x;
  int y1 = topLeft.y;
  int x2 = bottomRight.x;
  int y2 = bottomRight.y;
  Vec::Vec2D start(startPoint());
  Vec::Vec2D end(endPoint());
  switch (arcType) {
    case PIE:
      Pie(paintDC, x1, y1, x2, y2, start.x, start.y, end.x, end.y);
      break;
    case CHORD:
      Chord(paintDC, x1, y1, x2, y2, start.x, start.y, end.x, end.y);
      break;
    case ARC:
      Arc(paintDC, x1, y1, x2, y2, start.x, start.y, end.x, end.y);
      break;
  }
}

std::vector<POINT> LineArc::coords() const {
  std::vector<POINT> coordVector = BBoxCoords();
  coordVector.push_back(static_cast<POINT>(startPoint()));
  coordVector.push_back(static_cast<POINT>(endPoint()));
  return coordVector;
}
