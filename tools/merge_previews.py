"""Combine official single-board benches for the store image tool."""
from pathlib import Path
import json
import shutil
import sys

source, target = map(Path, sys.argv[1:3])
summaries = sorted(source.glob('watch-clock-*/summary.json'))
if not summaries:
    raise SystemExit('No firmware preview summaries found')
target.mkdir(parents=True, exist_ok=True)
merged = None
for path in summaries:
    report = json.loads(path.read_text())
    if merged is None:
        merged = {**report, 'screens': [], 'findings': []}
    if (report['plugin'], report['version']) != (merged['plugin'], merged['version']):
        raise SystemExit('Preview artifacts contain different plugin versions')
    merged['screens'].extend(report['screens'])
    merged['findings'].extend(report['findings'])
    for screen in report['screens']:
        key = screen['key']
        shutil.copytree(path.parent / key, target / key, dirs_exist_ok=True)
(target / 'summary.json').write_text(json.dumps(merged, indent=2), encoding='utf-8')
warnings = [f for f in merged['findings'] if f['level'] == 'warning']
print(f"Combined {len(merged['screens'])} boards, {len(warnings)} warnings")
if warnings:
    raise SystemExit(1)
