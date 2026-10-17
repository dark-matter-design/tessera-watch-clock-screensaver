"""Check generated source and compile/run the actual C++ geometry tests."""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from generate_numerals import header, PATHS, sample

ROOT = Path(__file__).resolve().parents[1]
assert (ROOT/'components/watch_clock/numerals.h').read_text(encoding='utf-8') == header(), 'Run tools/generate_numerals.py'
assert [len(sample(p)) for p in PATHS] == [3,26,73,49,61]
compiler = shutil.which('c++') or shutil.which('g++') or shutil.which('clang++')
if not compiler:
    sys.exit('Numeral generation passed; native tests require a C++17 compiler. Run in Linux/WSL or GitHub Actions.')
with tempfile.TemporaryDirectory() as temp:
    binary=Path(temp)/('geometry.exe' if sys.platform=='win32' else 'geometry')
    subprocess.run([compiler,'-std=c++17','-Wall','-Wextra','-I',str(ROOT),str(ROOT/'tests/geometry_test.cpp'),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
