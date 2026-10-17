"""Install plugin-specific runtime checks into an otherwise unchanged official bench."""
from pathlib import Path
import shutil

root = Path(__file__).resolve().parents[1]
tools = root / '.research/tessera-plugins/tools'
shutil.copyfile(root / 'tools/bench_checks.py', tools / 'watch_clock_bench_checks.py')
path = tools / 'bench.py'
source = path.read_text(encoding='utf-8')
marker = "if __name__ == '__main__':"
hook = 'from watch_clock_bench_checks import install\ninstall(Bench, saver_word)\n\n'
if hook not in source:
    path.write_text(source.replace(marker, hook + marker), encoding='utf-8')
