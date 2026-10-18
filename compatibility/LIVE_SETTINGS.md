# Historical fork setup (versions 0.3.0 and 0.4.0 only)

**Watch Clock 0.4.1 uses standard Tessera API 0.14. Do not follow the fork installation instructions below for the current plugin.** Use the normal Tessera app and firmware, update the plugin and rebuild each screen. Remove any `smart_display` source override pointing at the fork before rebuilding. Hands tick once per second; options are read when standby starts. The following instructions are retained only as history.

Watch Clock 0.4.0 requires **your Tessera fork 0.4.90**, with firmware **0.58.0** and plugin API **0.15**. This release contains live device options, the interactive Clock preview, smooth hand callbacks and the large built-in Roboto digit step.

## Set up

1. In Home Assistant, open **Settings → Apps → App store → ⋮ → Check for updates**. Update your existing forked Tessera app to **0.4.90**, then start it and refresh its browser page. Its slug and data storage are unchanged; there is no need to uninstall or create another app.
2. Open Tessera and update Watch Clock to **0.4.0**.
3. Build/upload each screen once with the plugin update, bringing it to firmware **0.58.0**. If installing the plugin already uploaded 0.58.0, that screen is finished. At startup the app points ordinary screen profiles at this fork's `screens-v0.4.90` tag, including the components and fonts. A manual firmware override is normally unnecessary.
4. Choose Watch Clock under **Settings → Screensaver → Clock**. Settings update both the Clock illustration and the active device face. Select **Smooth (30 FPS)**, **Fluid (60 FPS)** or **Tick (1 FPS)**.

The normal profile migration changes only its recognised firmware package URL/reference and matching `GITHUB_REF`. Wi-Fi, API/OTA credentials, screen name, other packages and local overrides are preserved. Custom branches, commit pins and unrelated forks remain untouched. If you deliberately pinned a custom source, choose the fork's release yourself before building. An explicit old `smart_display` override can also take precedence; remove that old upstream-only entry or point it at this fork.

The fork repository is `https://github.com/dark-matter-design/homeassistant_espscreen`. The original MaxGramser app is a separate installation and will not receive this fork's update. If you do not see 0.4.90, check that you are looking at the app installed from this fork.

The screen build should now fetch `dark-matter-design/homeassistant_espscreen` at `screens-v0.4.90`, rather than the upstream API 0.14 sources. Updating the app does not flash the devices; run each screen's firmware update once afterwards.

Both historical source patches remain in this repository for reference. They are already included in the versioned fork release and do not need to be applied manually. The editor preview is a browser illustration; physical frame-rate performance still needs checking on the actual panels.
