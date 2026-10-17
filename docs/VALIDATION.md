# Validation

This beta requires Tessera plugin API 0.14. It must still be tested on a physical Tessera device before a stable release.

Automated checks:

- Official Tessera manifest and source checker.
- Native C++ geometry tests against the original renderer: cardinal directions, fractional hour/minute angles, square/adaptive sizing, 1.65 aspect cap, scale clamping, alignment and stroke bounds across supported display sizes.
- Generated numeral consistency check, including all cubic endpoints.
- GitHub Actions: official strict preview bench using actual Tessera firmware for CYD, square Guition, standing Waveshare and large Guition displays, in both screen themes. Results and images are uploaded even when the bench fails.
- Additional strict previews for both Guition 7-inch catalog boards, JC1060P470 and JC1060P470 V2 (1024 × 600). The Guition 4-inch square and 10.1-inch displays are covered by the default bench. These are desktop firmware builds, not hardware acceptance tests.

Physical device checklist (not performed):

- Add by repository link, build and select Watch Clock for the Clock step.
- Check date, scale, alignment, all accent colours and seconds toggle.
- Verify midnight, time correction, screen time zone, UTC and daylight-saving changes.
- Leave running for several hours; confirm drift, readable night brightness and stable memory.
- Verify touch wake, alerts, step changes, black-screen sleep and firmware update teardown.
- Repeat on CYD without PSRAM and on the intended larger panel.

`flash_kb` is an estimate pending a physical device build; the saver memory budget includes retained numeral points and eleven LVGL objects. A passing desktop bench does not verify real-panel refresh speed, electrical behaviour or device memory pressure.
