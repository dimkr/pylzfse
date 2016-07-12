import os
from distutils.core import setup, Extension

lzfse = Extension('lzfse', sources=[os.path.join('lzfse', 'src', x) for x in os.listdir(os.path.join('lzfse', 'src')) if x.endswith('.c') and x != 'lzfse_main.c'] + ['pylzfse.c'], extra_compile_args=['-std=c99'], include_dirs=[os.path.join('lzfse', 'src')])

setup(name='pylzfse',
      version='0.1',
      description='Python bindings for the LZFSE reference implementation',
      ext_modules=[lzfse])
