"""Bundle the browser preview with Tessera's own Roboto font (no external requests)."""
from pathlib import Path
import base64
import os

root = Path(__file__).resolve().parents[1]
tree = Path(os.environ.get('TESSERA_TREE', root / '.research/tessera'))
font = base64.b64encode((tree / 'fonts/Roboto-400.ttf').read_bytes()).decode()
script = (root / 'previews/watch.js').read_text(encoding='utf-8')
html = '''<!doctype html><html><head><meta charset="utf-8">
<meta http-equiv="Content-Security-Policy" content="default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; font-src data:; img-src data:; base-uri 'none'; form-action 'none'">
<style>
@font-face{font-family:WatchRoboto;src:url(data:font/ttf;base64,FONT) format('truetype')}
html,body{margin:0;width:100%;height:100%;overflow:hidden;background:#000}
canvas{display:block;width:100%;height:100%}
</style></head><body><canvas></canvas><script>SCRIPT</script></body></html>'''.replace('FONT', font).replace('SCRIPT', script)
(root / 'previews/watch.html').write_text(html, encoding='utf-8')
(root / 'licenses/Roboto-OFL.txt').write_bytes((tree / 'fonts/Roboto-OFL.txt').read_bytes())
print(f'watch.html: {len(html.encode())} bytes')
