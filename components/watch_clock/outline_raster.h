// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "numerals.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace esphome::watch_clock::outline {
using geometry::Outline;
struct Box { float x, y, width, height; };
inline Box place(const geometry::Layout &art, int index) {
  const float height = (index == 0 ? 74.0f : 82.0f) * art.unit;
  const float width = geometry::OUTLINES[index].width * height;
  const float inset = 4 * art.unit;
  float x = art.x + (art.width - width) / 2;
  float y = art.y + (art.height - height) / 2;
  if (index == 0) y = art.y + inset;
  if (index == 1) x = art.x + art.width - inset - width;
  if (index == 2) y = art.y + art.height - inset - height;
  if (index == 3) x = art.x + inset;
  return {x, y, width, height};
}
// Even/odd scan conversion preserves holes and disjoint contours. Four vertical
// samples and continuous horizontal coverage soften edges at the actual screen
// resolution. Fixed scratch space, no glyph bitmap, font engine or heap buffer.
struct Scan { float x[32]{}; int count = 0; };
inline Scan scan(const Outline &glyph, float y, float scale) {
  Scan out;
  size_t begin = 0;
  for (size_t c = 0; c < glyph.contours; ++c) {
    const size_t end = glyph.ends[c];
    auto previous = glyph.points[end - 1];
    for (size_t i = begin; i < end; ++i) {
      const auto p = glyph.points[i];
      if ((p.y <= y && previous.y > y) || (previous.y <= y && p.y > y)) {
        if (out.count == 32) return {};  // a corrupt/unexpected outline must not overrun scratch space
        out.x[out.count++] = (p.x + (y - p.y) * (previous.x - p.x) / (previous.y - p.y)) * scale;
      }
      previous = p;
    }
    begin = end;
  }
  std::sort(out.x, out.x + out.count);
  return out;
}

template<class Paint>
void paint(const Outline &glyph, float scale, int left, int top, int right, int bottom, Paint emit) {
  if (scale <= 0 || left > right || top > bottom) return;
  for (int y = top; y <= bottom; ++y) {
    Scan samples[4];
    for (int s = 0; s < 4; ++s) samples[s] = scan(glyph, (y + (s + .5f) / 4) / scale, scale);
    for (int base = left; base <= right; base += 64) {
      const int width = std::min(64, right - base + 1);
      uint16_t coverage[64]{};
      for (const auto &row : samples) for (int pair = 0; pair + 1 < row.count; pair += 2) {
        const float start = std::max<float>(base, row.x[pair]);
        const float end = std::min<float>(base + width, row.x[pair + 1]);
        for (int x = std::max(base, static_cast<int>(std::floor(start))); x < std::ceil(end); ++x) {
          const float amount = std::max(0.0f, std::min<float>(x + 1, end) - std::max<float>(x, start));
          coverage[x - base] += static_cast<uint16_t>(std::lround(amount * 255));
        }
      }
      for (int x = 0; x < width;) {
        const uint8_t alpha = (coverage[x] + 2) / 4;
        int end = x + 1;
        while (end < width && (coverage[end] + 2) / 4 == alpha) ++end;
        if (alpha) emit(base + x, base + end - 1, y, alpha);
        x = end;
      }
    }
  }
}
}  // namespace esphome::watch_clock::outline
