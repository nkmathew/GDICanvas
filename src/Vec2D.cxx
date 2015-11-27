#include "./Vec2D.h"

using namespace Vector;

Vec2D::Vec2D(int x_, int y_) {
  x = x_;
  y = y_;
}

Vec2D::Vec2D(float x_, float y_) {
  x = x_;
  y = y_;
}

Vec2D::Vec2D(const POINT &point) {
  x = point.x;
  y = point.y;
}

Vec2D::Vec2D() {
  x = 0.0;
  y = 0.0;
}

Vec2D::~Vec2D() {
}

float Vec2D::toRadians(float angle) {
  return angle * PI / 180.0;
}

// ~~~~~~~~~~~~~~~~~~~~~~[ Addition ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Vec2D Vec2D::operator+(float scalar) {
  return Vec2D(x + scalar, y + scalar);
}

Vec2D Vec2D::operator+(const Vec2D &other) {
  return Vec2D(other.x + x, other.y + y);
}

// ~~~~~~~~~~~~~~~~~~~~~~[ Subtraction ]~~~~~~~~~~~~~~~~~~~~~~~~~~~
Vec2D Vec2D::operator-(float scalar) {
  return Vec2D(x * scalar, y * scalar);
}

Vec2D Vec2D::operator-() {
  return Vec2D(-x, -y);
}

Vec2D Vec2D::operator-(const Vec2D &other) {
  return Vec2D(x - other.x, y - other.y);
}

// ~~~~~~~~~~~~~~~~~~~~~~[ Multiplication ]~~~~~~~~~~~~~~~~~~~~~~~
Vec2D Vec2D::operator*(const Vec2D &other) {
  return Vec2D(x * other.x, y * other.y);
}

Vec2D Vec2D::operator*(float scalar) {
  return Vec2D(x * scalar, y * scalar);
}

// ~~~~~~~~~~~~~~~~~~~~~~[ Comparison ]~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Vec2D::operator<=(const Vec2D &other) {
  return other.x <= x && other.y <= y;
}

bool Vec2D::operator>=(const Vec2D &other) {
  return other.x >= x && other.y >= y;
}

bool Vec2D::operator!=(const Vec2D &other) {
  return !(*this == other);
}

bool Vec2D::operator==(const Vec2D &other) {
  float deltaX = std::abs(other.x - x);
  float deltaY = std::abs(other.y - y);
  return (deltaX <= FLT_EPSILON) && (deltaY <= FLT_EPSILON);
}

Vec2D::operator POINT() {
  return {static_cast<long>(x), static_cast<long>(y)};
}

// cppcheck-suppress unusedFunction
Vec2D Vec2D::rotate(float angle) {
  angle = toRadians(angle);
  float newX, newY;
  newX = x * std::cos(angle) - y * std::sin(angle);
  newY = x * std::sin(angle) + y * std::cos(angle);
  return Vec2D(newX, newY);
}

Vec2D Vec2D::abs() {
  return Vec2D(std::abs(x), std::abs(y));
}

char *Vec2D::str() {
  snprintf(coordRepr, 50, "(%.2f, %.2f)", x, y);
  return coordRepr;
}

float Vec2D::magnitude(float x, float y) {
  return magnitude(Vec2D(x, y));
}

float Vec2D::magnitude(const Vec2D &other) {
  Vec2D vector(*this - other);
  return std::sqrt(std::pow(vector.x, 2.0) + std::pow(vector.y, 2.0));
}
