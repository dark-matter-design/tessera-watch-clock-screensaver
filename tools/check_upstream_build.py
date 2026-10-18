"""Generate ESP32 firmware from standard Tessera URLs with Watch Clock API 0.14.

Uses disposable configuration and no device secrets. Nothing is flashed.
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
folder = root / '.research/upstream-codegen'
folder.mkdir(parents=True, exist_ok=True)
text = installation_yaml({'board': 'guition', 'name': 'watch-codegen', 'friendly_name': 'Watch code generation'})
assert REPO == 'https://github.com/MaxGramser/homeassistant_espscreen'
assert f'url: {REPO}' in text
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
print('Standard upstream firmware source and Watch Clock API code generation: ok')
