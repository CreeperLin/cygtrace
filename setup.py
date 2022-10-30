import os
import sys
from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

root_dir = os.path.abspath(os.path.dirname(__file__))
sys.path.append(root_dir)
readme = open(os.path.join(root_dir, 'README.md')).read()
requirements = [
    name.rstrip()
    for name in open(os.path.join(root_dir, 'requirements.txt')).readlines()
]
version = open(os.path.join(root_dir, 'VERSION')).read()
try:
    git_head = open(os.path.join(root_dir, '.git', 'HEAD')).read().split()[1]
    git_version = open(os.path.join(root_dir, '.git', git_head)).read()[:7]
    version += ('+git' + git_version)
except Exception:
    pass

pybind11_ext = Pybind11Extension(
    'cygtrace',
    sorted(glob('*.cpp') + glob('*.c')),
    define_macros=[('VERSION_INFO', version)],
)

pybind11_ext.extra_compile_args += ['-finstrument-functions']
pybind11_ext.extra_link_args += ['-export-dynamic']

ext_modules = [pybind11_ext]

setup(
    name='cygtrace',
    version=version,
    author='Yunfeng Lin',
    author_email='linyunfeng@sjtu.edu.cn',
    url='https://github.com/CreeperLin/cygtrace',
    description='gcc instrumentation based tracer for c/c++/pybind11',
    long_description=readme,
    long_description_content_type='text/markdown',
    install_requires=requirements,
    tests_require=['pytest'],
    py_modules=['cygtrace'],
    cmdclass={'build_ext': build_ext},
    ext_modules=ext_modules,
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
        'Topic :: Software Development :: Libraries',
        'Topic :: Utilities',
    ],
    python_requires='>=3.6',
)
