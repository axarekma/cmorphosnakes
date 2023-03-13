from setuptools import setup, Extension
from Cython.Distutils import build_ext
import numpy as np
from distutils.sysconfig import customize_compiler
import os

NAME = "cmorphosnakes"
VERSION = "0.1"
DESCR = "Morphosnakes using Cython"
REQUIRES = ["numpy", "cython", "pytest"]
AUTHOR = "Axel Ekman"
EMAIL = "axel.a.ekman@gmail.com"

SRC_DIR = "cmorphosnakes"
PACKAGES = [SRC_DIR]

os.environ["CC"] = "g++-12"
os.environ["CXX"] = "g++-12"


class build_ext_ex(build_ext):
    extra_compile_args = {"unix": ["-fopenmp"], "msvc": ["/openmp"]}
    extra_linker_args = {"unix": ["-fopenmp"]}

    def build_extension(self, ext):
        ctype = self.compiler.compiler_type
        extra_c_args = self.extra_compile_args.get(ctype, [])
        extra_l_args = self.extra_linker_args.get(ctype, [])

        if extra_c_args is not None:
            ext.extra_compile_args = extra_c_args

        if extra_l_args is not None:
            ext.extra_link_args = extra_l_args

        print(f"build_extension {ext}")
        print(f"ctype:   {ctype}")
        print(f"extra_c_args:   {extra_c_args}")
        print(f"extra_l_args:   {extra_l_args}")

        # fix cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++
        # solution from
        # https://stackoverflow.com/questions/8106258/cc1plus-warning-command-line-option-wstrict-prototypes-is-valid-for-ada-c-o
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass

        build_ext.build_extension(self, ext)


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
        cmdclass={"build_ext": build_ext_ex},
        ext_modules=EXTENSIONS,
    )
