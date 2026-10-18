# Changelog

## 0.4.2 - 2026-10-18
- Restores the original large vector numerals, which scale with the face without custom Tessera fonts or firmware.
- Keeps standard Tessera API 0.14 compatibility and once-per-second ticking.
- Documents standard Tessera's option-refresh and text-only plugin-preview limitations.

## 0.4.1 - 2026-10-18
- Restores compatibility with standard Tessera 0.4.89 and plugin API 0.14; no fork or firmware override is required.
- Hands update once per second. Removes the 30/60 FPS setting and animation interpolation.
- Uses standard built-in numeral fonts and reads face options when the screensaver opens, without fork-only callbacks.
- Keeps the face colours, coloured background, date sizes, layouts, alignment and burn-in drift.

## 0.4.0 - 2026-10-18
- Restores oversized numerals with the core's new large built-in Roboto digit step, rendered at native resolution.
- Updates the interactive preview and firmware screenshots to match.
- Requires the versioned Tessera fork update 0.4.90 (firmware 0.58.0), which fixes automatic firmware source selection.

## 0.3.0 - 2026-10-18
- Replaces outlined vector numerals with Tessera's built-in digit font.
- Adds smooth 30 FPS hands by default, with 1 FPS and 60 FPS choices. Frame rates are requests, subject to hardware performance.
- Applies all face options while the saver is visible, with matching live-options support in the Tessera fork.
- Adds an interactive Clock preview that follows the selected settings, through the matching Tessera editor integration.
- Requires the API 0.15 animation firmware plus the live-options patch in compatibility/. Update the firmware once to enable these features.

## 0.2.0 - 2026-10-18
- The date is larger by default and follows the face colour. Adds Small, Medium and Large date sizes with automatic fitting.
- Adds Coloured background: the chosen colour fills the glass, with black numerals and date and contrasting hands.
- Keeps burn-in movement on the clock artwork while the background stays full screen without black edges.
- Updates the firmware screenshots and store image for the new appearance.

## 0.1.0 - 2026-10-18
- First Tessera port of the Watch Clock screensaver with original vector numerals and white hands.
- Adds original and adaptive layouts, scale, alignment, theme accents and optional seconds and date.
- Uses Tessera's screensaver lifecycle, burn-in drift and screen time zone, with an optional UTC clock.
- Requires plugin API 0.14; smooth sweeping seconds and full-background colour presets are unavailable on Tessera.
