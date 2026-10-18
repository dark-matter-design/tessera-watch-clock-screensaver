# Validation

## Current version: 0.4.1, standard Tessera

Version 0.4.1 targets unmodified upstream Tessera 0.4.89, firmware 0.57.0, plugin API 0.14. CI pins the firmware
and checker to `MaxGramser/homeassistant_espscreen` revision `9f6ab3c68d420f454fb905466696063b1d789049`.
No animation callback, live-options callback, custom font step or fork patch is used.

Hands update through `on_tick(epoch)` once per second. Options are read when the screensaver opens, so wake the
screen and enter standby again after changing them. Numerals use standard built-in fonts and are smaller than the
fork-only oversized numerals. Colours, background inversion, date sizes, UTC, layout, alignment and drift remain.

The official manifest checker passes against upstream API 0.14. Geometry tests cover second ticks, scales, aspect
ratios and alignment. Current CI runs five strict real-firmware benches, generates store imagery and checks ESP32
code generation from standard upstream URLs. Its 80 screenshots cover eight scenarios and light/dark themes on
CYD, Guition 4-inch, both 7-inch models and the 10.1-inch model. Fork-only timer and live-settings tests are no longer
part of validation. Physical device checks remain necessary; a desktop render is not a hardware test.

## Historical 0.4.0 fork results

The results below apply only to the previous fork-dependent version. They do not establish standard-repository
compatibility, and the fork installation and FPS comparison steps are not required for 0.4.1.

Version 0.4.0 uses the API 0.15 animation/live-options callbacks and the new built-in large Roboto digit step in Tessera fork 0.4.90, firmware 0.58.0. The fork also migrates ordinary screen profiles to its own release sources. These are desktop builds and browser checks; physical panel refresh speed remains unmeasured.

## Completed results

On 18 October 2026 (Australia/Sydney), [CI run 39076442929](https://github.com/dark-matter-design/tessera-watch-clock-screensaver/actions/runs/39076442929) passed the official checker, native geometry/interpolation tests, host timer lifecycle tests, five strict real-firmware preview benches and store-image generation for source revision `0862858`.

The benches produced **80 screenshots with zero findings** across eight scenarios and both screen themes on:

- CYD, 320 × 240.
- Guition 4-inch square, 480 × 480.
- Guition 7-inch JC1060P470, 1024 × 600.
- Guition 7-inch JC1060P470 V2, 1024 × 600.
- Guition 10.1-inch JC8012P4A1, 1280 × 800.

Each profile also exercises live background inversion/restoration, date visibility, scale/alignment and animation speed over the normal device API. It checks that settings change the rendered image without recreating the saver; captures separate frames to confirm smooth hands move within a wall-clock second at both requested frame rates; and confirms Tick mode stops interpolation. This verifies animation behaviour, not sustained panel throughput.

Geometry tests cover fractional hand angles, second transitions, delayed frames, clock corrections, monotonic rollover, square/adaptive sizing, aspect cap, scale clamping and alignment. Host tests cover timer creation, retiming, disabling, restart and safe teardown, including a saver that closes inside its callback.

The small/large firmware images, date corners, normal/inverted faces and store picture were visually reviewed. Numerals use the core's large native-resolution Roboto digit step with cropped line metrics and no bitmap enlargement. The preview uses matching glyph metrics and weights, plus the same physical margins and burn-in drift. Browser-preview colour inversion, scale, date visibility and animation controls were inspected in a sandboxed iframe with Tessera's Roboto font.

The Tessera app changes passed 27 related editor tests, four focused backend preview tests, changed-file ESLint, source/test TypeScript checks and a production editor build. The original 20 screensaver backend regressions also passed. Real ESP32 code generation fetched the fork's package, components and fonts from GitHub and successfully registered Watch Clock through API 0.15, exercising the exact check that rejected upstream API 0.14. The final fork release [CI run 39076923988](https://github.com/dark-matter-design/homeassistant_espscreen/actions/runs/39076923988) also passed all 19 app checks on Linux: 1,626 Python tests (11 skipped), 54 C++ tests, package/generator/translation checks, editor tests/lint/types/build and the WebAssembly preview. Its committed editor bundle was verified fresh. See [preview integration](../compatibility/README.md).

## Physical checks remaining

- Install the matching Tessera fork/app and Watch Clock update; rebuild and upload each intended Guition screen once.
- Change all face options in standby and confirm both the device and editor preview respond.
- Compare 30 and 60 FPS on the 4-, 7- and 10-inch screens, including touch responsiveness and long-running behaviour.
- Check midnight, time corrections, UTC, screen time zone and daylight-saving changes.
- Verify burn-in drift, night brightness, wake, alerts, step changes and firmware-update teardown.

`flash_kb` is an estimate pending a device build. The conservative 4608-byte saver budget now holds eleven LVGL objects, hand point buffers and members; custom numeral point arrays have been removed. Browser HTML/font assets are app preview files and are not compiled into firmware.
