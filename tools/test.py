"""Compile/run the actual C++ geometry tests."""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
ROOT = Path(__file__).resolve().parents[1]
compiler = shutil.which('c++') or shutil.which('g++') or shutil.which('clang++')
if not compiler:
    sys.exit('Native tests require a C++17 compiler. Run in Linux/WSL or GitHub Actions.')
with tempfile.TemporaryDirectory() as temp:
    binary=Path(temp)/('geometry.exe' if sys.platform=='win32' else 'geometry')
    subprocess.run([compiler,'-std=c++17','-Wall','-Wextra','-I',str(ROOT),str(ROOT/'tests/geometry_test.cpp'),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
