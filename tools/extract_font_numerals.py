"""Extract filled numeral contours from a supplied OTF; the font is not bundled.

Usage: python tools/extract_font_numerals.py /path/to/SF-Pro-Rounded-Bold.otf
Requires FontTools only when regenerating the committed vector data.
"""
from pathlib import Path
import hashlib
import json
import sys
from fontTools.ttLib import TTFont
from fontTools.pens.basePen import BasePen

ROOT = Path(__file__).resolve().parents[1]


class Contours(BasePen):
    def __init__(self, glyphs, offset=0):
        super().__init__(glyphs)
        self.offset, self.paths, self.path = offset, [], []

    def _moveTo(self, p):
        self.path = [p]

    def _lineTo(self, p):
        self.path.append(p)

    def _curveToOne(self, a, b, c):
        def flatten(p, a, b, c, depth=0):
            dx, dy = c[0] - p[0], c[1] - p[1]
            length = (dx * dx + dy * dy) ** .5
            distance = max(abs(dx * (q[1] - p[1]) - dy * (q[0] - p[0])) / max(length, .001) for q in (a, b))
            if distance <= .5 or depth == 16:
                self.path.append(c)
                return
            mid = lambda u, v: ((u[0] + v[0]) / 2, (u[1] + v[1]) / 2)
            pa, ab, bc = mid(p, a), mid(a, b), mid(b, c)
            left, right = mid(pa, ab), mid(ab, bc)
            centre = mid(left, right)
            flatten(p, pa, left, centre, depth + 1)
            flatten(centre, right, bc, c, depth + 1)
        flatten(self.path[-1], a, b, c)

    def _closePath(self):
        if self.path and self.path[-1] == self.path[0]:
            self.path.pop()
        self.paths.append([(x + self.offset, y) for x, y in self.path])
        self.path = []

    _endPath = _closePath


def extract(source):
    font = TTFont(source)
    glyphs, cmap = font.getGlyphSet(), font.getBestCmap()
    result = []
    for text in ('12', '3', '6', '9'):
        paths, offset = [], 0
        for char in text:
            name = cmap[ord(char)]
            pen = Contours(glyphs, offset)
            glyphs[name].draw(pen)
            paths.extend(pen.paths)
            offset += font['hmtx'][name][0]
        xs = [x for path in paths for x, _ in path]
        ys = [y for path in paths for _, y in path]
        left, right, top, bottom = min(xs), max(xs), max(ys), min(ys)
        height = top - bottom
        result.append({'text': text, 'width': (right - left) / height,
                       'contours': [[[(x - left) / height, (top - y) / height] for x, y in path] for path in paths]})
    return result


def header(glyphs, digest):
    out = ['// Generated geometric outlines from the user-supplied SF-Pro-Rounded-Bold.otf.',
           '// Source SHA256: ' + digest, '// The OTF is not included. Regenerate with tools/extract_font_numerals.py.',
           '#pragma once', '#include "geometry.h"', '#include <cstddef>', '#include <cstdint>',
           'namespace esphome::watch_clock::geometry {',
           'struct Outline { const Point *points; const uint16_t *ends; size_t contours; float width; };']
    for n, glyph in enumerate(glyphs):
        out.append(f'inline constexpr Point OUTLINE_{n}[] = {{')
        ends, count = [], 0
        for path in glyph['contours']:
            out.extend(f'  {{{x:.7f}f, {y:.7f}f}},' for x, y in path)
            count += len(path)
            ends.append(count)
        out += ['};', f'inline constexpr uint16_t ENDS_{n}[] = {{{", ".join(map(str, ends))}}};']
    out.append('inline constexpr Outline OUTLINES[] = {')
    for n, glyph in enumerate(glyphs):
        out.append(f'  {{OUTLINE_{n}, ENDS_{n}, {len(glyph["contours"])}, {glyph["width"]:.7f}f}},')
    out += ['};', '}  // namespace esphome::watch_clock::geometry', '']
    return '\n'.join(out)


if __name__ == '__main__':
    source = Path(sys.argv[1])
    glyphs = extract(source)
    digest = hashlib.sha256(source.read_bytes()).hexdigest()
    (ROOT / 'components/watch_clock/numerals.h').write_text(header(glyphs, digest), encoding='utf-8')
    (ROOT / 'previews/numerals.json').write_text(json.dumps(glyphs, separators=(',', ':')), encoding='utf-8')
    print('Extracted:', ', '.join(f'{g["text"]}: {sum(map(len, g["contours"]))} points' for g in glyphs))
