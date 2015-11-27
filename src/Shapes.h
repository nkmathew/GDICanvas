/*!
 * \file Shapes.h
 * \brief Contains shape classes
 */

#ifndef Shapes_H_
#define Shapes_H_

#define _WIN32_WINNT 0x501

#include <windows.h>
#include <cstdio>
#include <algorithm>
#include <string>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <vector>
#include <memory>
#include "./Vec2D.h"
#include "./Colors.h"
#include <wingdi.h>

namespace Vec = Vector;

/*!
 * \namespace GShape
 * \brief Shape stuff
 */
namespace GShape {

/*!
 * \brief Takes the end points of two lines and returns their point of
 * intersection.
 *
 * \returns {LONG_MAX, LONG_MAX} if the lines don't intersect.
 */
Vec::Vec2D intersection(const Vec::Vec2D &start1,
                        const Vec::Vec2D &end1,
                        const Vec::Vec2D &start2,
                        const Vec::Vec2D &end2);

struct Shape;

//! Returns __true__ if the two shapes are equal. The function is used in
//! GC::Canvas::addShape
bool areEqual(const std::shared_ptr<Shape> &first,
              const std::shared_ptr<Shape> &second
             );

//! Returns \b true if the point is inside the rectangular region.
bool pointInRegion(int xCoord,
                   int yCoord,
                   const Vec::Vec2D &topLeft,
                   const Vec::Vec2D &bottomRight);

//! \overload pointInRegion
bool pointInRegion(const Vec::Vec2D &point,
                   const Vec::Vec2D &topLeft,
                   const Vec::Vec2D &bottomRight);

//! Returns \b true if the two rectangular regions share at least point.
bool regionsOverlap(const Vec::Vec2D &top1,
                    const Vec::Vec2D &bottom1,
                    const Vec::Vec2D &top2,
                    const Vec::Vec2D &bottom2);

//! Returns the top left coordinate in the list of coordinates
Vec::Vec2D topLeftCoord(const std::vector<POINT> &coords);

//! Returns the bottom right coordinate in the list of coordinates
Vec::Vec2D bottomRightCoord(const std::vector<POINT> &coords);

//! Used to identify the shape. It's used in GC::Canvas::shapeType.
enum ShapeType {
  //! Oval/ellipse
  OVAL,
  //! Circle
  CIRCLE,
  //! Polygon
  POLYGON,
  //! Text
  TEXT,
  //! Rectangle
  RECTANGLE,
  //! Line
  LINE,
  //! An arc
  LINE_ARC,
  //! For indicating errors
  INVALID_SHAPE
};

//! Checks if the point is with the given line distance.
bool withinLineSegment(const Vec::Vec2D &point,
                       const Vec::Vec2D &start,
                       const Vec::Vec2D &end);

//! \overload withinLineSegment
bool withinLineSegment(float x, float y,
                       const Vec::Vec2D &start,
                       const Vec::Vec2D &end);

//! Types of arcs that can be drawn
enum ArcType {
  //! A pie
  PIE,
  //! A chord(A pie without the triangular part)
  CHORD,
  //! An arc line
  ARC
};

/*!
 * \class ELLIPSE_POINTS
 * \brief Represents the two possible points on an ellipse circumference with
 * the same `x` or `y` coordinate.
 * \see Shape::ordinateToCoord
 * \see Shape::abscissaToCoord
 */
struct ELLIPSE_POINTS {
  //! First possible point to the left/right/top/bottom of the axis
  Vec::Vec2D first;
  //! The other possible point to the left/right/top/bottom of the axis
  Vec::Vec2D second;
};

/*!
 * \enum BorderStyle
 * \brief Shape border/pen style
 */
enum BorderStyle {
//! PS_SOLID in __wingdi.h__
  SOLID,
//! PS_DASH in __wingdi.h__
  DASH,
//! PS_DOT in __wingdi.h__
  DOT,
//! PS_DASHDOT in __wingdi.h__
  DASHDOT,
//! PS_DASHDOTDOT in __wingdi.h__
  DASHDOTDOT,
//! PS_NONE in __wingdi.h__
  NONE,
//! For indicating errors
  INVALID_BORDER,
};


/*!
 * \struct FontAttr
 * \brief Holds the shape's font attributes
 */
struct FontAttr {
  //! Font family
  std::string family = "Consolas";
  //! Font size
  int size = 12;
  //! Bold style. Only one bold style is used. WinAPI's got lots of them
  int bold = FW_NORMAL;
  //! Underlines the text
  bool underline = false;
  //! Draws a line across the text
  bool strikeout = false;
  //! Slants the text
  bool italic = false;
  //! Returns the values of all the attributes in a string
  std::string repr() {
    char text[100];
    snprintf(text, 100,
             "family: `%s', size: %d, bold: %d, underline: %d, strikeout: %d, italic: %d",
             family.c_str(), size, bold, underline, strikeout, italic);
    return text;
  }
};

/*!
 * \class Shape
 * \brief A class to represent the items drawn on the canvas.
 */
class Shape {
    //! The shape's font attributes
    FontAttr fontProp;

    //! Determines whether the shape will be shown/drawn.
    bool isDrawn = true;

    //! Used to assign shape IDs
    static int counterID;

    //! Specifies the shape's background color in hex notation
    std::string fillColor = "";

    //! Specifies the shape's outline color in hex notation
    std::string penColor = "#000000";

    //! Stores all the tags associated with the shape. A tag is a shape's alias.
    std::vector<std::string> tagList = {"all"};

    //! Pen style
    BorderStyle border = SOLID;

    //! The string to be displayed in text objects/shapes
    std::string text = "";

  public:
    ShapeType shapeType = INVALID_SHAPE;

    //! Changes the shape's font attributes
    void setFontAttr(const FontAttr &fontProp);

    //! Returns the shape's text
    std::string getText();

    //! Changes the shape's text
    void setText(const std::string &text_);

    //! Returns the shape's font attributes
    FontAttr getFontAttr();

    //! Returns the border style
    BorderStyle borderStyle();

    //! Set border style
    void borderStyle(BorderStyle style);

    //! Adds the tag to tagList
    bool addTag(const std::string &newTag);

    //! Removes the tag from the tagList. All tags can be removed except "all"
    void removeTag(const std::string &tag);

    //! Returns all tags
    std::vector<std::string> tags();

    //! Returns the shape type
    ShapeType type();

    //! Returns true if the shape is drawn on the canvas.
    bool isShown();

    //! Toggles the shape's visibility on the canvas
    void visibility(bool visible);

    //! Top left corner of the bounding box.
    Vec::Vec2D topLeft = {0, 0};

    //! Bottom right corner of the bounding box.
    Vec::Vec2D bottomRight = {0, 0};

    //! Controls the shape's border
    int penSize = 0;

    //! Identifies the shape uniquely
    int shapeID;

    /*!
     * \brief Sets fill color.
     * \param[in] fillColor_ The hex color string. An empty string turns off
     *  filling.
     */
    void setFillColor(std::string fillColor_);

    //! Sets pen color
    void setPenColor(std::string penColor_);

    //! Returns the shape's pen color
    std::string getPenColor();

    //! Returns the shape's fill color
    std::string getFillColor();

    //! Returns the width of the bounding box
    int BBoxLength();

    //! Returns the height of the bounding box
    int BBoxHeight();

    //! Returns \b true if the shape has tag \p tagName
    bool hasTag(std::string tagName);

    //! Returns a struct `{x, y}` representing the bounding box's center.
    Vec::Vec2D BBoxCenter();

    //! Returns \b true if the coordinate is within the ellipse's circumference
    bool pointInEllipse(int x, int y);

    /*!
     *  \brief Returns \b true if the ellipse shares a point with the region.
     *
     *  The method is used in the ellipse derived shapes, LineArc and Oval
     */
    bool ellipseOverlapsRegion(const Vec::Vec2D &topLeft,
                               const Vec::Vec2D &bottomRight);

    /*!
     * \brief Check if any of the four vertices of the region are inside the
     * shape.
     *
     * Used to test if shape shares a point with a region.
     */
    bool regionOverlapsShape(const Vec::Vec2D &topLeft,
                             const Vec::Vec2D &bottomRight);

    /*!
     * \brief Returns \b true if the shape's bounding box shares a point with
     * the specified region.
     *
     * The region is rectangular
     * \param[in] topLeft The top left corner's coordinate
     * \param[in] bottomRight The bottom right corner's coordinate
     */
    bool BBoxOverlapsRegion(const Vec::Vec2D &topLeft,
                            const Vec::Vec2D &bottomRight);

    /*!
     * \brief Returns a struct `{Vec::Vec2D, Vec::Vec2D}` representing the two
     * possible points that have the same y value.
     *
     * \returns `{{x1, y1}, {x2, y2}}` on success
     * \returns `{{LONG_MAX, y}, {LONG_MAX, y}}` on failure
     *
     * \see abscissaToCoord()
     */
    ELLIPSE_POINTS ordinateToCoord(float y);

    /*!
     * \brief Returns a struct `{Vec::Vec2D, Vec::Vec2D}` representing the two
     * possible points that have the same x value.
     *
     * \returns `{{x1, y1}, {x2, y2}}` on success
     * \returns `{{x, LONG_MAX}, {x, LONG_MAX}}` on failure
     *
     * \see ordinateToCoord()
     */
    ELLIPSE_POINTS abscissaToCoord(float x);

    //! Returns the coordinate of the bottom right vertex of the bounding box
    virtual Vec::Vec2D bottomRightCoord() const = 0;

    //! Returns coordinate of the bottom right vertex of the bounding box
    virtual Vec::Vec2D topLeftCoord() const = 0;

    /*! Returns the closest vertex in the shape to coordinate `(x, y)`
     *
     *  The method is used by Canvas::tagClosest
     */
    virtual Vec::Vec2D closestPointTo(int x, int y);

    //! \overload bool pointInShape(int x1, int y) = 0;
    virtual bool pointInShape(const Vec::Vec2D &point);

    //! Returns \b true if the point `(x, y)` is within the shape.
    //! Used in mouse click events.
    virtual bool pointInShape(int x1, int y) = 0;

    /*!
     * \brief Returns \b true if the shape shares a point with the region
     * specified.
     *
     * The method is used in Canvas::tagOverlapping
     */
    virtual bool overlapsWithRegion(const Vec::Vec2D &top,
                                    const Vec::Vec2D &bottom) = 0;

    /*!
     * \brief Returns \b true if the whole shape is within the specified region.
     */
    virtual bool shapeInRegion(const Vec::Vec2D &topLeft,
                               const Vec::Vec2D &bottomRight);

    /*!
     * \brief Responsible for drawing the shapes on the screen using *WinAPI's*
     * *GDI* functions.
     *
     * The function is called in response to a `WM_PAINT` message.
     * The message is handled by Canvas::handleMessage.
     */
    virtual void draw(HDC paintDC) = 0;

    /*!
     * \brief Returns a vector with all the shape's points
     *
     * Shapes desribed by their bounding boxes will have coordinates ordered
     * from the top left vertex moving clockwise.
     */
    virtual std::vector<POINT> coords() const;

    /*!
     * \brief Changes a shapes coordinates.
     */
    virtual void changeCoords(const std::vector<POINT> &coords);

    //! Moves the shape by the specified amount
    virtual void move(int xAmount, int yAmount);

    //! \see LineArc::coords
    std::vector<POINT> BBoxCoords() const;

    virtual ~Shape() {}

    explicit Shape(ShapeType shapeType_) {
      shapeType = shapeType_;
      shapeID = counterID++;
    }
};

/*!
 * \class Poly
 * \brief Represents a Polygon
 */
struct Poly : Shape {
  std::vector<POINT> polyCoords;
  POINT coordinates[1000];
  float yCoords[1000];
  float xCoords[1000];
  virtual std::vector<POINT> coords() const override;
  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  virtual Vec::Vec2D closestPointTo(int x, int y) override;
  virtual bool pointInShape(int x_, int y_) override;
  virtual void draw(HDC paintDC) override;
  virtual void move(int xAmount, int yAmount) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  virtual bool shapeInRegion(const Vec::Vec2D &topLeft,
                             const Vec::Vec2D &bottomRight) override;

  virtual void changeCoords(const std::vector<POINT> &coords) override;
  explicit Poly(const std::vector<POINT> &points) : Shape(POLYGON) {
    addTag("polygon");
    polyCoords = points;
  }
};

/*!
 * \class Rect
 * \brief Represents a Rectangle
 */
struct Rect : Shape {
  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  virtual bool pointInShape(int x, int y) override;
  virtual void draw(HDC paintDC) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  Rect(int x1, int y1, int x2, int y2) : Shape(RECTANGLE) {
    addTag("rectangle");
    topLeft = {x1, y1};
    bottomRight = {x2, y2};
  }
};

/*!
 * \class Text
 * \brief Represents Text drawn on the screen.
 */
struct Text : Shape {
  Vec::Vec2D start;
  int width = 0;

  /*!
   * \note The values returned by this function will not always be accurate.
   * The functions used to query the string size depend on a DC(device context)
   * which is only sent to the draw method. Using GetDC(NULL) doesn't seem to work.
   *
   * The only time when it'll be correct is when the query is made in a handler,
   * i.e after the Canvas starts translating messages.
   *
   * Don't expect something like this to be accurate:
   *
   *  \code
   *    int main() {
   *      Canvas canv;
   *      canv.init()
   *      int text = canv.text(100, 300, "Hello world");
   *      canv.rectangle(canv.BBox(text)); // The bounding box will be smaller
   *      return canv.loop();
   *    }
   *  \endcode
   */
  virtual Vec::Vec2D bottomRightCoord() const override;

  virtual Vec::Vec2D topLeftCoord() const override;
  virtual bool pointInShape(int x, int y) override;

  //! Returns the size of the text area
  POINT textArea(HDC paintDC);
  void createFont(HFONT *font);
  virtual void move(int xAmount, int yAmount) override;

  /*!
   * \brief Draws the text on the screen.
   *
   * It doesn't support wrapping because it's quite difficult to determine the
   * extent of the text when the font and the different styles are factored in.
   *
   */
  virtual void draw(HDC paintDC) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  Text(int x, int y, const std::string &text_, int width_) : Shape(TEXT) {
    addTag("text");
    width = (width_ < 0) ? 0 : width_;
    setText(text_);
    start = {x, y};
  }
};

/*!
 * \class Oval
 * \brief Represents an Oval
 */
struct Oval : Shape {
  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  virtual bool pointInShape(int x, int y) override;
  virtual void draw(HDC paintDC) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  Oval(int x1, int y1, int x2, int y2) : Shape(OVAL) {
    addTag("oval");
    topLeft = {x1, y1};
    bottomRight = {x2, y2};
  }
};

/*!
 * \class Circle
 * \brief Represents a Circle
 */
struct Circle : Oval {
  Vec::Vec2D center;
  int radius;
  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  virtual void draw(HDC paintDC) override;

  /*!
   * The first coordinates is taken to be the center and the x-coordinates of
   * the second point to be the new radius.
   *
   * e.g changeCoords({{12, 13}, {20 ,300}});
   *      radius = 20;
   *      center = {12, 13};
   */
  void changeCoords(const std::vector<POINT> &coords);
  void updateBBoxCoords();
  Circle(int x, int y, int rad) : Oval(x - rad, y - rad, x + rad, y + rad) {
    addTag("circle");
    center = {x, y};
    radius = rad;
  }
};

/*!
 * \class Line
 * \brief Represents a Line
 */
struct Line : Shape {
  std::vector<POINT> lineCoords;

  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  virtual void draw(HDC paintDC) override;
  virtual bool pointInShape(int x, int y) override;
  virtual std::vector<POINT> coords() const override;
  virtual void move(int xAmount, int yAmount) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  virtual bool shapeInRegion(const Vec::Vec2D &topLeft,
                             const Vec::Vec2D &bottomRight) override;

  bool pointInLine(const Vec::Vec2D &p1,
                   const Vec::Vec2D &p2,
                   float x,
                   float y);

  virtual void changeCoords(const std::vector<POINT> &coords) override;
  explicit Line(const std::vector<POINT> &points) : Shape(LINE) {
    addTag("line");
    lineCoords = points;
  }
};

/*!
 * \class LineArc
 * \brief Represents an arc
 */
struct LineArc : Shape {
  ArcType arcType;
  float pieSize = 60.0f;
  float tiltAngle = 60.0f;

  //! Returns the point where the arc drawing starts from
  Vec::Vec2D startPoint() const;

  //! Returns the point where the arc drawing stops from
  Vec::Vec2D endPoint() const;

  /*!
   * \brief Takes the point \p (x, y) and returns the angle that point is,
   * measured from the right half of the center line.
   */
  float angleFromCoord(float x, float y);

  //! \see http://stackoverflow.com/questions/2049582
  float sign(const Vec::Vec2D &p1, const Vec::Vec2D &p2, const Vec::Vec2D &p3);

  /*!
   * \brief Returns \b true if the point is in the triangle.
   *
   * \see http://stackoverflow.com/questions/2049582/
   */
  bool pointInTriangle(const Vec::Vec2D &point,
                       const Vec::Vec2D &v1,
                       const Vec::Vec2D &v2,
                       const Vec::Vec2D &v3);

  /*!
   * \brief Returns \b true if the point is on the arc line.
   */
  bool pointOnArc(int x, int y);
  bool pointOnArc(const Vec::Vec2D &point);

  /*!
   * \brief Returns \b true if the point is within the pie region.
   */
  bool pointInPie(int x, int y);

  /*!
   * \brief Returns \b true if the point is within the pie region.
   */
  bool pointInChord(int x, int y);

  /*!
   * \brief Returns the point in the circumference at the specified angle.
   *
   * The angle is measured from the right half of the center line.
   * \param[in] angle from 0 to \b +inf or \b -inf.
   *
   * The point returned is placed in the right sector.
   */
  Vec::Vec2D coordFromAngle(float angle) const;

  //! Returns \b true if the arc shares a point with the region.
  bool arcOverlapsRegion(const Vec::Vec2D &topLeft_,
                         const Vec::Vec2D &bottomRight_);

  //! Returns \b true if the chord shares a point with the region.
  bool chordOverlapsRegion(const Vec::Vec2D &topLeft_,
                           const Vec::Vec2D &bottomRight_);

  //! Returns \b true if the pie shares a point with the region.
  bool pieOverlapsRegion(const Vec::Vec2D &topLeft_,
                         const Vec::Vec2D &bottomRight_);

  /*!
   * \brief The vector will be filled with the coordinates of every vertex not
   * just for its bounding box. The order:
   *
   * `{top-left, top-right, bottom-right, bottom-left, start-point, end-point}`
   * \code
   *   canv.arc(100, 150, 510, 500, CHORD);
   *   std::vector<POINT> points;
   *   canv.coords(points);
   *   for (auto point : points) {
   *     printf("(%li, %li)", point.x, point.y);
   *   }
   * \endcode
   *
   *     (100, 150) (510, 150) (510, 500) (100, 500) (510, 325) (395, 168)
   *
   */
  virtual std::vector<POINT> coords() const override;

  virtual Vec::Vec2D bottomRightCoord() const override;
  virtual Vec::Vec2D topLeftCoord() const override;
  bool pointInShape(const Vec::Vec2D &point);
  virtual bool pointInShape(int x, int y) override;
  virtual void draw(HDC paintDC) override;

  virtual bool overlapsWithRegion(const Vec::Vec2D &topLeft,
                                  const Vec::Vec2D &bottomRight) override;

  LineArc(int x1,
          int y1,
          int x2,
          int y2,
          ArcType arcType_ = CHORD,
          float pieSize_ = 60.0f,
          float tiltAngle_ = 0.0f) : Shape(LINE_ARC) {
    addTag("arc");
    arcType = arcType_;
    pieSize = pieSize_;
    tiltAngle = tiltAngle_;
    topLeft = {x1, y1};
    bottomRight = {x2, y2};
  }
};

/*!
 * \struct Box
 * \brief Used to represent a certain region using the top left and bottom right
 * coordinates
 *
 * It's similar as WinAPI's RECT only that the coordinates are of type float.
 */
struct Box {
  //! top left corner's coordinates
  float x1, y1;

  //! bottom right corner's coordinates
  float x2, y2;

  Box() {
    x1 = 0.0f;
    x2 = 0.0f;
    y1 = 0.0f;
    y2 = 0.0f;
  }

  Box(float x1_, float y1_, float x2_, float y2_) {
    x1 = x1_;
    y1 = y1_;
    x2 = x2_;
    y2 = y2_;
  }

  Box(long x1_, long y1_, long x2_, long y2_) {
    // Created to suppress narrowing conversion warnings with uniform
    // initializers.
    x1 = x1_;
    y1 = y1_;
    x2 = x2_;
    y2 = y2_;
  }

  // cppcheck-suppress noExplicitConstructor
  Box(RECT rect) {
    // For conversions from WinAPI's RECT struct
    x1 = rect.left;
    y1 = rect.top;
    x2 = rect.right;
    y2 = rect.bottom;
  }
};

}

#endif
