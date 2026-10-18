# Watch Clock for Tessera

A native screensaver face for Tessera, ported from [Watch Clock for Kiosk Satellite](https://github.com/dark-matter-design/kiosk-satellite-watch-clock-screensaver). The large 12, 3, 6 and 9 use filled vector outlines extracted from the supplied SF Pro Rounded Bold font, with rounded contrasting hands and a coloured date. The OTF file is not bundled. No browser, custom firmware font, image download or Android component is needed.

![Watch Clock rendered by the real Tessera firmware](store.png)

Full firmware previews: [black-background face](docs/previews/guition-4-inch-black.png), [4-inch square](docs/previews/guition-4-inch.png), [7-inch](docs/previews/guition-7-inch.png), [10.1-inch adaptive](docs/previews/guition-10-inch-adaptive.png).

**Works with standard Tessera 0.4.89 / firmware 0.57.0 (API 0.14).** Version 0.5.0 adds filled rounded numeral outlines with coverage-based edge smoothing. No Tessera fork, compatibility patch or firmware-source override is needed.

## Set up

1. Open **Plugins** in Tessera and choose **Add with a link**.
2. Paste `https://github.com/dark-matter-design/tessera-watch-clock-screensaver`.
3. Add Watch Clock to your screen and build its firmware.
4. Open that screen's **Settings  ->  Screensaver**, open the **Clock** step and choose **Watch Clock** under **Face**.
5. Enable **Auto standby**, use a standby brightness above zero and put Clock where you want it in the screensaver order. Any earlier active step takes priority.
6. Adjust the face options in the Clock drawer. The plugin reads them when the screensaver opens; wake the screen and let it enter standby again after changing them. Touching the face wakes the screen normally.

For folder installation, copy the manifest, `plugin.yaml`, `components/`, `translations/`, README and changelog into `/config/tessera-plugins/watch_clock/` beside Home Assistant's `esphome` folder, then add it from Tessera.

## Options

| Option | Choices / default |
| --- | --- |
| Face colour | Green (default), White, Blue, Purple, Amber; colours both numerals and date |
| Coloured background | Off by default; on fills the glass with the face colour and uses black numerals and date |
| Layout | Original proportions (default), Adaptive spacing |
| Scale | 50-100%, default 100% |
| Alignment | Centre (default), Left, Right, Top, Bottom |
| Seconds hand | On by default |
| Show date | On by default |
| Date size | Large (default), Medium, Small; automatically fits longer formats and small scales |
| Date position | Bottom left (default), Bottom right, Top left, Top right |
| Date format | Short weekday and day (default), Day and short month, Numeric YYYY-MM-DD |
| Time zone | Screen time zone (default, follows Tessera including DST), UTC |

Adaptive spacing extends the composition on the longer axis, capped at 1.65 times its square dimension. Large numerals retain the supplied glyph proportions and scale with the clock. Their filled contours include the holes in 6 and 9. Adaptive subdivision limits curve error; scan conversion uses fractional horizontal coverage and four vertical samples per pixel at the actual display size. Scratch space is bounded: there is no full-glyph bitmap or runtime font engine. The date uses standard Tessera fonts. A small inset preserves Tessera's burn-in movement at every alignment. Long dates can use free space above or below a scaled clock to stay clear of the numerals. Until the screen's clock is synchronised, the numerals remain visible and the hands and date are hidden.

## Differences from Kiosk Satellite

Hands tick once per second through Tessera's standard `on_tick` callback. There is no smooth-sweep or FPS setting. The default face uses a black background. Turn on **Coloured background** for the source-style green background with black numerals and date. Hands stay white on coloured faces, with black hands on the white face for contrast. Custom hex colour pickers are unavailable. Time zones are Screen or UTC; choose an IANA time zone in Tessera's screen settings. Numeric dates use ISO order. This clock does not display the Clock step's temperature or entity row.

Tessera owns activation, brightness, night hours, step selection, wake, alerts and firmware updates. The plugin adds a face to the Clock step, with no extra permissions or network access. The artwork follows Tessera's burn-in drift while its solid background stays fixed to fill the glass. Boards that do not support Tessera standby cannot show it.

## Development and validation

The project was generated with Tessera's official plugin template. Clone the [plugin tools](https://github.com/MaxGramser/tessera-plugins) beside this repository, then run:

```sh
pip install pyyaml
python ../tessera-plugins/tools/check.py .
python tools/test.py
python ../tessera-plugins/tools/preview.py . --strict
python ../tessera-plugins/tools/store_image.py . --from .preview
```

Use an unmodified checkout of `MaxGramser/homeassistant_espscreen` with API 0.14. CI pins upstream revision `9f6ab3c68d420f454fb905466696063b1d789049` (Tessera 0.4.89). `TESSERA_TREE`, `TESSERA_MANIFEST` and `TESSERA_BOARDS` may point at that checkout for reproducible validation. `python tools/generate_preview.py` bundles the optional browser illustration with standard Tessera's Roboto font. Historical fork-only bench helpers are not used by current CI.

The official preview bench needs Linux/macOS or WSL with a C++ compiler and SDL2. GitHub Actions runs the manifest checker, geometry tests and real firmware preview on five board profiles in parallel. Its downloadable `watch-clock-preview` artifact contains rendered screens and findings. See [validation notes](docs/VALIDATION.md) for test coverage and hardware limitations.

The additional Guition checks cover the 7-inch JC1060P470 and V2 (1024 x 600), alongside the square 4-inch ESP32-S3-4848S040 (480 x 480) and 10.1-inch JC8012P4A1 (1280 x 800). Both 7-inch boards support standby in Tessera's catalog; their hardware support is marked experimental upstream.

The plugin is installed from source; no Kiosk Satellite ZIP, DEX or APK is used. Without a GitHub release, Tessera follows the default branch as a test installation. Releases are intentionally deferred until a real screen has been tested.

## Credits

Original watch proportions: dark-matter-design's Kiosk Satellite Watch Clock, Apache-2.0. Numeral contour data: the user-supplied SF Pro Rounded Bold OTF, converted into geometric data without bundling the font. Tessera plugin scaffolding: MaxGramser's Tessera plugin template, MIT. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Independent community plugin.

## Preview in Tessera

The plugin listing uses the generated `store.png` from the loaded plugin revision. Update the plugin to 0.5.0 and refresh Tessera to load its image. Standard Tessera's Clock drawer displays only an icon and the plugin's example text; it does not load `store.png` or the optional browser illustration there. Updating the plugin cannot change that panel. See the Plugins page or the full firmware screenshots linked above for the actual appearance. The optional editor integration under [compatibility/](compatibility/README.md) is historical and is not required to install or run this version.

To regenerate the numeral data, install FontTools and run `python tools/extract_font_numerals.py /path/to/SF-Pro-Rounded-Bold.otf`.
The extractor records the input SHA256 in `numerals.h`. Only the contours for 12, 3, 6 and 9 are committed. The optional
browser illustration reads the same contours; `tools/generate_preview.py` also requires FontTools to subset its separate
Roboto date fonts. The original centreline generator is retained as historical tooling and is not used for 0.5.0.

Standard Tessera also passes a plugin's options only when the saver opens. Changes made while it is visible will
appear after waking the screen and re-entering standby, or toggling the Clock step off and on. The plugin receives
neither new options nor an options-change callback while active, so it cannot refresh them by itself through API 0.14.
