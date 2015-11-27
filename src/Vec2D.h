/*!
 * \file Vec2D.h
 * \brief Provides a 2Dimensional Vector class
 */

#ifndef Vec2D_H_
#define Vec2D_H_

#include <windows.h>
#include <cmath>
#include <string>
#include <cstdio>
#include <cfloat>

#define PI 3.14159265

/*!
 * \namespace Vector
 * \brief Vec2D stuff
 */
namespace Vector {

//! A 2-Dimensional vector class
struct Vec2D {
  float x, y;
  Vec2D(float x_, float y_);
  Vec2D(int x_, int y_);
  Vec2D();
  // cppcheck-suppress noExplicitConstructor
  Vec2D(const POINT &points);

  //! String representation of the coordinates e.g `"(125.25, 35.23)"`
  char coordRepr[50];

  //! Makes both `x` and `y` positive
  Vec2D abs();

  //! Calculates distance between two points
  float magnitude(const Vec2D &other);

  //! \overload magnitude
  float magnitude(float x, float y);

  //! Rotate the angle in anticlockwise rotation
  Vec2D rotate(float angle);

  //! Convert angle from degrees to radians
  float toRadians(float angle);

  Vec2D operator*(const Vec2D &other);
  Vec2D operator*(float scalar);

  Vec2D operator+(const Vec2D &other);
  Vec2D operator+(float scalar);

  Vec2D operator-();
  Vec2D operator-(const Vec2D &other);
  Vec2D operator-(float scalar);

  bool operator<=(const Vec2D &other);
  bool operator>=(const Vec2D &other);

  bool operator==(const Vec2D &other);
  bool operator!=(const Vec2D &other);

  // Casting to struct tagPOINT alias POINT, defined as {LONG x, LONG y}
  operator POINT();

  char *str();
  virtual ~Vec2D();
};
}
#endif
