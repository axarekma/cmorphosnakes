from setuptools import setup, Extension
from Cython.Distutils import build_ext
import numpy as np

NAME = "cmorphosnakes"
VERSION = "0.1"
DESCR = "Morphosnakes using Cython"
REQUIRES = ["numpy", "cython"]
AUTHOR = "Axel Ekman"
EMAIL = "axel.a.ekman@gmail.com"

SRC_DIR = "cmorphosnakes"
PACKAGES = [SRC_DIR]

ext_1 = Extension(
    SRC_DIR + "._morphosnakes_fm",
    [SRC_DIR + "/_morphosnakes_fm.pyx"],
    libraries=[],
    include_dirs=[np.get_include()],
    language="c++",
)

EXTENSIONS = [ext_1]

if __name__ == "__main__":
    setup(
        install_requires=REQUIRES,
        packages=PACKAGES,
        zip_safe=False,
        name=NAME,
        version=VERSION,
        description=DESCR,
        author=AUTHOR,
        author_email=EMAIL,
        cmdclass={"build_ext": build_ext},
        ext_modules=EXTENSIONS,
    )
