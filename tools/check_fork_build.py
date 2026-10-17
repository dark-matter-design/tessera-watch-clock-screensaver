"""Generate real ESP32 firmware from fork URLs, including this plugin.

This exercises the same register_plugin/API check that failed on the user's build.
It uses disposable configuration and no real device secrets; no firmware is flashed.
"""
from pathlib import Path
import os
import subprocess
import sys

root = Path(__file__).resolve().parents[1]
tree = Path(os.environ['TESSERA_TREE'])
sys.path.insert(0, str(tree / 'screen_manager/app'))
from core import installation_yaml, REPO  # noqa: E402

pin = os.environ['TESSERA_CODEGEN_REF']
folder = root / '.research/fork-codegen'
folder.mkdir(parents=True, exist_ok=True)
text = installation_yaml({'board': 'guition', 'name': 'watch-codegen', 'friendly_name': 'Watch code generation'})
assert f'url: {REPO}' in text and 'dark-matter-design' in REPO
text = text.replace('    ref: main', f'    ref: {pin}')
text = text.replace('substitutions:\n', f'substitutions:\n  GITHUB_REF: "{pin}"\n', 1)
text += f'''
external_components:
  - source:
      type: local
      path: "{(root / 'components').as_posix()}"
    components: [watch_clock]
watch_clock:
'''
(folder / 'watch-codegen.local.yaml').write_text('{}\n')
(folder / 'secrets.yaml').write_text('wifi_ssid: "Compile validation"\nwifi_password: "ValidationOnly123"\n')
config = folder / 'watch-codegen.yaml'
config.write_text(text, encoding='utf-8')
subprocess.run([sys.executable, '-m', 'esphome', 'compile', '--only-generate', str(config)], check=True)
print('Fork firmware source and Watch Clock API code generation: ok')
