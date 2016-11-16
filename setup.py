from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    ext_modules = cythonize([Extension("atimacpy", ["atimacpy.pyx"], libraries=["gfortran","m"], extra_objects=["libatimac.a","libdedx.a","libbspline.a","libcolsys.a","libcmlib.a"],library_dirs=[r'./atima.1.3/lib'],)])
)
