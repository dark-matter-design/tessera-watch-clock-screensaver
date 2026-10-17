# Tessera Clock preview integration

`tessera-saver-preview.patch` is a source patch against Tessera dev revision `b18ee2f`. It has not been submitted upstream. A copy is included on your Tessera fork's `codex/live-saver-options` branch alongside the firmware callback change.

The plugin listing uses `store.png`. The Clock drawer uses `previews/watch.html` for a live illustration and falls back to the firmware screenshot `previews/watch.png` when HTML is unavailable, then its icon/example. The HTML uses Tessera's bundled Roboto font and mirrors the native face's options, layout, colours and hand animation. This is a browser illustration, not firmware execution or a prediction of the panel's achieved FPS. Its time follows the editor's clock (UTC when selected); the physical face follows Tessera's device clock.

The backend fetches only a declared saver's asset at its pinned plugin commit, with separate HTML/PNG caches. HTML is bounded to 512 KiB and runs in an opaque-origin sandbox without network, parent-page access or device actions. The editor passes only the face's options, screen dimensions/font metrics, language and preview time. Missing HTML leaves the screenshot visible. No manifest changes are required.

## Set up

Use the fork's review branch for both app and firmware, as described in [LIVE_SETTINGS.md](LIVE_SETTINGS.md). The patch copy is retained here for review or manual application to a Tessera checkout:

```sh
git switch dev
git apply --check /path/to/tessera-watch-clock-screensaver/compatibility/tessera-saver-preview.patch
git apply /path/to/tessera-watch-clock-screensaver/compatibility/tessera-saver-preview.patch
cd web
npm ci
npm test
npm run build
```

Build the add-on from that source checkout. Updating only the plugin or screen firmware cannot change Tessera's editor.

## Validation

27 related editor tests passed, including frame isolation, current options/dimensions, immediate option messages, screenshot fallback and plugin-revision retry. Four focused backend tests passed for pinned fetching/caching, declared saver paths, missing/invalid PNG, invalid UTF-8/HTML and oversize documents. Source/test TypeScript checks, changed-file lint and the production editor build passed. Browser checks cover colour inversion, scale/date controls and animated hands inside the sandbox.

The wider Python catalogue suite contains Linux-only `os.fchmod` operations and cannot run unmodified on Windows. The patch changes AGPL-3.0-or-later Tessera source; its license is in `licenses/Tessera-AGPL-3.0.txt`. The firmware plugin is Apache-2.0; the embedded Roboto font is OFL-1.1, in `licenses/Roboto-OFL.txt`.
