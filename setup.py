import os
import sys
import subprocess
import tempfile
import glob
from distutils.command.install_headers import install_headers
from setuptools import setup
try:
    import pybind11
    from pybind11.setup_helpers import Pybind11Extension, build_ext
except ImportError:
    pybind11 = None
from distutils.core import Extension

root_dir = os.path.abspath(os.path.dirname(__file__))
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

cmdclass = {
    'install_headers': install_headers,
}

if pybind11 is not None:
    pybind11_ext = Pybind11Extension(
        'cygtrace',
        ['cygtrace_pybind11.cpp'],
        define_macros=[('VERSION_INFO', version)],
    )
    ext_modules = [pybind11_ext]
    cmdclass['build_ext'] = build_ext
else:
    python_ext = Extension(
        'cygtrace',
        sources=[
            'cygtrace_python.c',
        ],
    )
    ext_modules = [python_ext]

headers = glob.glob('*.h')

package_data = {
    'cygtrace_lib': [],
    'cygtrace_lib.lib': ['libcygtrace.so'],
    'cygtrace_lib.include': ['*.h'],
    'cygtrace_lib.share.pkgconfig': ['*.pc'],
    # 'cygtrace_lib.share.cmake.cygtrace': [],
}
packages = list(package_data.keys())

with tempfile.TemporaryDirectory(dir=root_dir) as tmpdir:
    cmd = ['cmake', '-S', '.', '-B', tmpdir] + [
        '-DCYGTRACE_BUILD_TESTS=OFF',
        '-DCYGTRACE_BUILD_EXAMPLES=OFF',
    ]
    if 'CMAKE_ARGS' in os.environ:
        cmd += [
            c
            for c in os.environ['CMAKE_ARGS'].split()
            if 'DCMAKE_INSTALL_PREFIX' not in c
        ]
    subprocess.run(cmd, check=True, cwd=root_dir, stdout=sys.stdout, stderr=sys.stderr)
    subprocess.run(['cmake', '--build', tmpdir], check=True, cwd=root_dir, stdout=sys.stdout, stderr=sys.stderr)
    subprocess.run(['cmake', '--install', tmpdir, '--prefix', 'cygtrace_lib'],
                   check=True, cwd=root_dir, stdout=sys.stdout, stderr=sys.stderr)

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
    cmdclass=cmdclass,
    ext_modules=ext_modules,
    headers=headers,
    packages=packages,
    package_data=package_data,
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: POSIX :: Linux',
        'Topic :: Software Development :: Libraries',
        'Topic :: Software Development :: Quality Assurance',
        'Topic :: Software Development :: Bug Tracking',
        'Topic :: System :: Logging',
    ],
    python_requires='>=3.6',
    entry_points={
        'console_scripts': [
            'cygtrace = cygtrace_lib:cygtrace_main',
        ]
    },
)
