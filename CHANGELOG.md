# Changelog

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
