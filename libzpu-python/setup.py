from distutils.core import setup, Extension

libzpu = Extension('zpu', sources = ['src/libpyzpu.c'], libraries = ['cintelhex','zpu'])

setup(name = 'zpu', version = '1.0', description = 'A library for controlling a Raggedston ZPU board.', ext_modules = [libzpu])
