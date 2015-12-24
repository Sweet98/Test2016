#!/usr/bin/env python
"""
This setup.py is used to build the python to a single exe file
Just run setip.py to build, the exe file will be output to 'dist' folder
"""
from distutils.core import setup
import py2exe, sys, os

sys.argv.append('py2exe')

setup(
    options = {'py2exe': {'bundle_files': 1, 'compressed': True}},
    console = [{'script': "AppEncrypt.py"}],
    zipfile = None,
)
