// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <algorithm>
#include <cmath>
namespace esphome::watch_clock::geometry {
struct Point { float x, y; };
struct Layout { float x, y, width, height, unit, dx, dy; };
inline Layout layout(float width, float height, float scale, bool adaptive, int alignment) {
  scale = std::isfinite(scale) ? std::clamp(scale, 50.0f, 100.0f) / 100.0f : 1.0f;
  const float side = std::min(width, height) * scale;
  const float w = adaptive ? std::min(width * scale, side * 1.65f) : side;
  const float h = adaptive ? std::min(height * scale, side * 1.65f) : side;
  float x = (width - w) / 2, y = (height - h) / 2;
  if (alignment == 1) x = 0;
  if (alignment == 2) x = width - w;
  if (alignment == 3) y = 0;
  if (alignment == 4) y = height - h;
  const float unit = side / 220;
  return {x, y, w, h, unit, (w / unit - 220) / 2, (h / unit - 220) / 2};
}
inline Point hand(float cx, float cy, float length, float degrees) {
  const float radians = degrees * 3.14159265358979323846f / 180;
  return {cx + std::sin(radians) * length, cy - std::cos(radians) * length};
}
struct Angles { float hour, minute, second; };
inline Angles angles(int hour, int minute, int second) {
  const float m = minute + second / 60.0f;
  return {(hour % 12 + m / 60) * 30, m * 6, second * 6.0f};
}
}  // namespace esphome::watch_clock::geometry
