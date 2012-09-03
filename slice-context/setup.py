#!/usr/bin/env python

from distutils.core import setup, Extension
import sys
sys.path.append("src")
import vsys

ext = Extension('_vsys', sources = ['src/vsys.c'])

setup(  name        = 'python-vsys',
        version     = '0.1',
        description = 'Python functions to wrap PlanetLab vsys API',
        long_description = vsys.__doc__,
        author      = 'Alina Quereilhac',
        author_email = 'alina.quereilhac@inria.fr',
        url         = 'http://nepi.inria.fr/code/python-vsys',
        platforms   = 'Linux',
        package_dir = {'': 'src'},
        ext_modules = [ext],
        py_modules  = ['vsys'])
