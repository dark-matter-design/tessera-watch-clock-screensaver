# Validation

Version 0.2.0 requires Tessera plugin API 0.14. The updated face still needs testing on physical screens before a stable release.

## Completed results

On 18 October 2026 (Australia/Sydney), [CI run 39073679000](https://github.com/dark-matter-design/tessera-watch-clock-screensaver/actions/runs/39073679000) passed the official checker, native C++ geometry tests, five strict real-firmware preview benches and store image generation for source revision `dced190`.

The benches produced **80 screenshots with zero findings**: eight scenarios in both screen themes for each of these profiles:

- CYD, 320 x 240.
- Guition 4-inch square, 480 x 480.
- Guition 7-inch JC1060P470, 1024 x 600.
- Guition 7-inch JC1060P470 V2, 1024 x 600.
- Guition 10.1-inch JC8012P4A1, 1280 x 800.

Scenarios cover the green background with black numerals and date, the black background with coloured date, adaptive spacing, hidden seconds/date, UTC numeric date, a white background at 50% clock scale, the narrow top-left date corner, and a top-aligned small clock with a long numeric date.

The store image, normal and inverted faces, small numeric date, and top-left date cases were visually inspected. Every coloured-background screenshot was also checked at all four corners to confirm it fills the glass without black seams. Long numeric dates move into free space when they cannot fit beside the large numerals. The artwork uses Tessera's drift while the solid background stays fixed.

Geometry tests cover cardinal hand directions, fractional hour/minute angles, square/adaptive sizing, the 1.65 aspect cap, scale clamping, alignment and stroke bounds across display sizes. Generated numeral consistency is checked separately. These are desktop builds of the real firmware, not physical panel tests.

The optional editor preview patch has its own tests and limitations documented in [compatibility/README.md](../compatibility/README.md). It has not been applied to Tessera or submitted upstream.

## Physical checks remaining for 0.2.0

- Update and rebuild each intended Guition screen, then test the larger coloured date and all face colours.
- Toggle Coloured background, including white, and check the contrasting hands.
- Check date positions, formats, sizes, scale and alignment.
- Verify midnight, time correction, screen time zone, UTC and daylight-saving changes.
- Leave running for several hours; confirm drift, readable night brightness and stable memory.
- Verify touch wake, alerts, step changes, black-screen sleep and firmware update teardown.

`flash_kb` remains an estimate pending a device build. The 4608-byte saver budget includes its retained points, members and twelve LVGL objects. Desktop previews do not verify real-panel refresh speed, electrical behaviour or device memory pressure.
