from pathlib import Path

import psutil

process = [proc for proc in psutil.process_iter() if proc.name() == "GeometryDash.exe"][0]

from pyinjector import inject

inject(process.pid, str(Path(__file__).absolute().parent / 'wsXbot.dll'))