# Validation

Version 0.3.0 uses the API 0.15 animation callbacks plus the live-options addition on the Tessera fork's `codex/live-saver-options` branch. The interactive preview also requires that branch's app changes. These are desktop builds and browser checks; physical panel refresh speed remains unmeasured.

## Completed results

On 18 October 2026 (Australia/Sydney), [CI run 39074771622](https://github.com/dark-matter-design/tessera-watch-clock-screensaver/actions/runs/39074771622) passed the official checker, native geometry/interpolation tests, host timer lifecycle tests, five strict real-firmware preview benches and store-image generation for source revision `3d0acad`.

The benches produced **80 screenshots with zero findings** across eight scenarios and both screen themes on:

- CYD, 320 × 240.
- Guition 4-inch square, 480 × 480.
- Guition 7-inch JC1060P470, 1024 × 600.
- Guition 7-inch JC1060P470 V2, 1024 × 600.
- Guition 10.1-inch JC8012P4A1, 1280 × 800.

Each profile also exercises live background inversion/restoration, date visibility, scale/alignment and animation speed over the normal device API. It checks that settings change the rendered image without recreating the saver; captures separate frames to confirm smooth hands move within a wall-clock second at both requested frame rates; and confirms Tick mode stops interpolation. This verifies animation behaviour, not sustained panel throughput.

Geometry tests cover fractional hand angles, second transitions, delayed frames, clock corrections, monotonic rollover, square/adaptive sizing, aspect cap, scale clamping and alignment. Host tests cover timer creation, retiming, disabling, restart and safe teardown, including a saver that closes inside its callback.

The small/large firmware images, date corners, normal/inverted faces and store picture were visually reviewed. Numerals use native Tessera font sizes with no bitmap enlargement. Browser-preview colour inversion, scale, date visibility and animation controls were inspected in a sandboxed iframe with Tessera's Roboto font.

The Tessera app changes passed 27 related editor tests, four focused backend preview tests, changed-file ESLint, source/test TypeScript checks and a production editor build. The original 20 screensaver backend regressions also passed. The wider catalogue suite has Linux-only `os.fchmod` calls and needs Linux before upstream integration. See [preview integration](../compatibility/README.md).

## Physical checks remaining

- Install the matching Tessera fork/app and Watch Clock update; rebuild and upload each intended Guition screen once.
- Change all face options in standby and confirm both the device and editor preview respond.
- Compare 30 and 60 FPS on the 4-, 7- and 10-inch screens, including touch responsiveness and long-running behaviour.
- Check midnight, time corrections, UTC, screen time zone and daylight-saving changes.
- Verify burn-in drift, night brightness, wake, alerts, step changes and firmware-update teardown.

`flash_kb` is an estimate pending a device build. The conservative 4608-byte saver budget now holds eleven LVGL objects, hand point buffers and members; custom numeral point arrays have been removed. Browser HTML/font assets are app preview files and are not compiled into firmware.
