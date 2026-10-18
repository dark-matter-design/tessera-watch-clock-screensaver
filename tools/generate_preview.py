"""Bundle the browser preview with Tessera's own Roboto font (no external requests)."""
from pathlib import Path
import base64
import os
import json
import io
from fontTools import subset
from fontTools.ttLib import TTFont

root = Path(__file__).resolve().parents[1]
tree = Path(os.environ.get('TESSERA_TREE', root / '.research/tessera'))
def preview_font(weight):
    font = TTFont(tree / f'fonts/Roboto-{weight}.ttf')
    glyphs = subset.Subsetter()
    glyphs.populate(text='0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ -/')
    glyphs.subset(font)
    out = io.BytesIO()
    font.save(out)
    return base64.b64encode(out.getvalue()).decode()
fonts = {weight: preview_font(weight) for weight in (400, 500, 700)}
script = (root / 'previews/watch.js').read_text(encoding='utf-8')
points = json.loads((root / 'previews/numerals.json').read_text())
script = script.replace('/* NUMERAL_POINTS */ []', json.dumps(points, separators=(',', ':')))
html = '''<!doctype html><html><head><meta charset="utf-8">
<meta http-equiv="Content-Security-Policy" content="default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; font-src data:; img-src data:; base-uri 'none'; form-action 'none'">
<style>
@font-face{font-family:WatchRoboto;font-weight:400;src:url(data:font/ttf;base64,FONT400) format('truetype')}
@font-face{font-family:WatchRoboto;font-weight:500;src:url(data:font/ttf;base64,FONT500) format('truetype')}
@font-face{font-family:WatchRoboto;font-weight:700;src:url(data:font/ttf;base64,FONT700) format('truetype')}
html,body{margin:0;width:100%;height:100%;overflow:hidden;background:#000}
canvas{display:block;width:100%;height:100%}
</style></head><body><canvas></canvas><script>SCRIPT</script></body></html>'''.replace('SCRIPT', script)
for weight, font in fonts.items():
    html = html.replace(f'FONT{weight}', font)
assert len(html.encode()) <= 512 * 1024
(root / 'previews/watch.html').write_text(html, encoding='utf-8')
(root / 'licenses/Roboto-OFL.txt').write_bytes((tree / 'fonts/Roboto-OFL.txt').read_bytes())
print(f'watch.html: {len(html.encode())} bytes')
