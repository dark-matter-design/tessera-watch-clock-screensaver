# Live options and smooth hands

Watch Clock 0.3.0 uses API 0.15 from [your Tessera fork](https://github.com/dark-matter-design/homeassistant_espscreen). The earlier animation change is already on its main branch. `tessera-live-options.patch` adds `Saver::on_options()` and updates the host timer when animation speed changes. The plugin paints the objects it already has; it never creates a timer or rebuilds the face during a settings change.

The corresponding firmware work is on the fork's `codex/live-saver-options` branch. Merge that branch into your fork's main before installing this plugin version, or select that branch in the override below while testing. The branch also includes the live editor-preview integration. Both patch copies remain in this repository.

## Set up

1. Merge [`codex/live-saver-options`](https://github.com/dark-matter-design/homeassistant_espscreen/compare/main...codex/live-saver-options) into your fork's main branch. In Home Assistant's App store, check for updates and **Rebuild** your existing forked Tessera app to load its updated editor/backend. Keep using that existing app and its data.
2. Open each screen's **More → Override YAML** and include:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/dark-matter-design/homeassistant_espscreen.git
      ref: main
      path: components
    components: [smart_display]
    refresh: 0s
```

3. Update Watch Clock to 0.3.0, then build and upload each screen's firmware once.
4. Open **Settings → Screensaver → Clock**, choose Watch Clock and change its options while it is visible. They update when Tessera delivers the saved setting.
5. Choose **Hand animation → Smooth (30 FPS)**, **Fluid (60 FPS)** or **Tick (1 FPS)**. This changes every hand's cadence. Achieved frame rate needs checking on your physical 4-, 7- and 10-inch boards.

For firmware testing before merging, use `ref: codex/live-saver-options` instead. The app's Clock preview responds to the chosen options and animation mode. Its PNG fallback remains a static example.
