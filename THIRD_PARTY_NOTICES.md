# Third-party notices

## Supplied numeral outline source

The filled numeral contours in `components/watch_clock/numerals.h` and `previews/numerals.json` were extracted from
[SF-Pro-Rounded-Bold.otf](https://raw.githubusercontent.com/sahibjotsaggu/San-Francisco-Pro-Fonts/master/SF-Pro-Rounded-Bold.otf),
supplied by the user. The source file is not bundled. Its SHA256 is recorded in the generated header. The extraction
script and scan-conversion renderer are plugin code; the numeral geometry derives from that supplied font.

## Kiosk Satellite Watch Clock

The large numeral outlines and sampled native polylines are also adapted from the Apache-2.0 source below.

The hand proportions and layout mathematics are adapted from [dark-matter-design/kiosk-satellite-watch-clock-screensaver](https://github.com/dark-matter-design/kiosk-satellite-watch-clock-screensaver), revision `f81debb5edf5405116f6b2cf51a2245e0ab77c9a`.

Licensed under Apache License 2.0, reproduced in LICENSE. Modified for native LVGL rendering, Tessera's theme, screen clock and standard screensaver lifecycle. No Java SDK or Android assets are included.

Copyright 2026 dark-matter-design. Port modifications copyright 2026 dark-matter-design.

## Tessera plugin template

The ESPHome component scaffold, plugin package pattern and assistant instructions were generated from [MaxGramser/tessera-plugins](https://github.com/MaxGramser/tessera-plugins), revision `85112ffc7d9146d33fe6ee22c6f3faec710ed80a`.

The MIT license and its copyright notice are reproduced in `licenses/Tessera-MIT.txt`.

## Tessera editor compatibility patch

`compatibility/tessera-saver-preview.patch` modifies source from MaxGramser/homeassistant_espscreen, dev revision `b18ee2f`. This patch is AGPL-3.0-or-later, separately from the Apache-2.0 firmware plugin. The upstream license is reproduced in `licenses/Tessera-AGPL-3.0.txt`.

## Roboto browser-preview font

The self-contained `previews/watch.html` embeds Tessera's Roboto Regular, Medium and Bold fonts, from `fonts/Roboto-400.ttf`, `Roboto-500.ttf` and `Roboto-700.ttf`, under SIL Open Font License 1.1. Its license is reproduced in `licenses/Roboto-OFL.txt`. It adds no font to the device firmware.
