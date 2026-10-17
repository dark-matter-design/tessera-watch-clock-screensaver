# Watch Clock for Tessera

A native screensaver face for Tessera, ported from [Watch Clock for Kiosk Satellite](https://github.com/dark-matter-design/kiosk-satellite-watch-clock-screensaver). The 12, 3, 6 and 9 use Tessera's built-in digit font, with rounded contrasting hour and minute hands, a fine seconds hand and a larger coloured date. No browser, external font, image download or Android component is needed.

![Watch Clock rendered by the real Tessera firmware](store.png)

Full firmware previews: [black-background face](docs/previews/guition-4-inch-black.png), [4-inch square](docs/previews/guition-4-inch.png), [7-inch](docs/previews/guition-7-inch.png), [10.1-inch adaptive](docs/previews/guition-10-inch-adaptive.png).

**Requires the Tessera fork with API 0.15 animation and live-options support.** See [firmware setup](compatibility/LIVE_SETTINGS.md). Stock Tessera API 0.14 cannot build this version.

## Set up

1. Open **Plugins** in Tessera and choose **Add with a link**.
2. Paste `https://github.com/dark-matter-design/tessera-watch-clock-screensaver`.
3. Add Watch Clock to your screen and build its firmware.
4. Open that screen's **Settings  ->  Screensaver**, open the **Clock** step and choose **Watch Clock** under **Face**.
5. Enable **Auto standby**, use a standby brightness above zero and put Clock where you want it in the screensaver order. Any earlier active step takes priority.
6. Adjust the face options in the Clock drawer. Changes apply to the active face as Tessera sends them; no wake or rebuild is needed for option changes. Touching the face wakes the screen normally.

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
| Hand animation | Smooth (30 FPS, default), Fluid (60 FPS), Tick (1 FPS) |
| Show date | On by default |
| Date size | Large (default), Medium, Small; automatically fits longer formats and small scales |
| Date position | Bottom left (default), Bottom right, Top left, Top right |
| Date format | Short weekday and day (default), Day and short month, Numeric YYYY-MM-DD |
| Time zone | Screen time zone (default, follows Tessera including DST), UTC |

Adaptive spacing extends the composition on the longer axis, capped at 1.65 times its square dimension. Hands keep the source proportions. Numerals use the largest built-in font that fits, at its native size without bitmap enlargement. A small inset preserves Tessera's burn-in movement at every alignment. Long dates can use free space above or below a scaled clock to stay clear of the numerals. The date uses the largest built-in font that fits the selected size and corner; weekday and month abbreviations come from the plugin's translations (English included). Until the screen's clock is synchronised, the numerals remain visible and the hands and date are hidden.

## Differences from Kiosk Satellite

The fork's API 0.15 drives a smooth sweep using elapsed time between wall-clock seconds. Its host owns and stops the animation timer when the saver closes. 30 FPS is the default; 60 FPS requests more rendering work, and physical frame rate depends on the board. The default face uses a black background. Turn on **Coloured background** for the source-style green background with black numerals and date. Hands stay white on coloured faces, with black hands on the white face for contrast. Custom hex colour pickers are unavailable. Time zones are Screen or UTC; choose an IANA time zone in Tessera's screen settings. Numeric dates use ISO order. This clock does not display the Clock step's temperature or entity row.

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

The official preview bench needs Linux/macOS or WSL with a C++ compiler and SDL2. GitHub Actions runs the manifest checker, geometry tests and real firmware preview on five board profiles in parallel. Its downloadable `watch-clock-preview` artifact contains rendered screens and findings. See [validation notes](docs/VALIDATION.md) for test coverage and hardware limitations.

The additional Guition checks cover the 7-inch JC1060P470 and V2 (1024 x 600), alongside the square 4-inch ESP32-S3-4848S040 (480 x 480) and 10.1-inch JC8012P4A1 (1280 x 800). Both 7-inch boards support standby in Tessera's catalog; their hardware support is marked experimental upstream.

The plugin is installed from source; no Kiosk Satellite ZIP, DEX or APK is used. Without a GitHub release, Tessera follows the default branch as a test installation. Releases are intentionally deferred until a real screen has been tested.

## Credits

Original watch proportions: dark-matter-design's Kiosk Satellite Watch Clock, Apache-2.0. Tessera plugin scaffolding: MaxGramser's Tessera plugin template, MIT. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Independent community plugin; no proprietary Apple font or artwork is included.

## Preview in Tessera

The plugin listing uses the generated `store.png` from the loaded plugin revision. Update the plugin to 0.3.0 and refresh Tessera to load the new image; old pinned revisions may lack the store picture. The Screensaver → Clock editor currently replaces every third-party face with its icon and example text. A plugin cannot replace that editor component through the current manifest API. The [prepared Tessera editor integration patch](compatibility/README.md) is kept in this repository and has not been submitted upstream or applied to your add-on.
