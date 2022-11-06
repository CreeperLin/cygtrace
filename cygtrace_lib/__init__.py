import argparse
import subprocess
import sys
import os
import json

root_dir = os.path.abspath(os.path.dirname(__file__))


def cygtrace_call(argv):
    root_dir = os.path.abspath(os.path.dirname(__file__))
    lib = os.path.join(root_dir, 'lib', 'libcygtrace.so')
    args = [f'LD_PRELOAD={lib}']
    args += argv
    return subprocess.call(' '.join(args), shell=True)
    # return os.system(' '.join(args))


def cygtrace_demangle(path):
    demangler = 'c++filt {name}'
    demangled = {}
    with open(path, 'r') as f:
        trace = json.load(f)
    for ev in trace['traceEvents']:
        name = ev['name']
        if name == '<unknown>':
            continue
        cmd = demangler.format(name=name)
        d_name = demangled.get(name)
        if d_name is None:
            d_name = subprocess.run(cmd, shell=True, capture_output=True).stdout.decode().strip()
            demangled[name] = d_name
        ev['name'] = d_name
    output = os.path.splitext(path)[0] + '.demangled.json'
    with open(output, 'w') as f:
        json.dump(trace, f)


def get_includes():
    return '-I' + os.path.join(root_dir, 'include')


def get_libs():
    return '-L' + os.path.join(root_dir, 'lib')


def get_params(compiler='gcc', linking=True):
    args = []
    if compiler == 'gcc':
        if linking:
            args.append('-lcygtrace')
        args.extend(['-finstrument-functions', '-Wl,--export-dynamic'])
    else:
        raise NotImplementedError
    return ' '.join(args)


def cygtrace_main():
    parser = argparse.ArgumentParser('cygtrace')
    parser.add_argument('-d', '--demangle', action='store_true')
    parser.add_argument('-I', '--includes', action='store_true')
    parser.add_argument('-L', '--libs', action='store_true')
    parser.add_argument('-m', '--module', action='store_true')
    parser.add_argument('-p', '--params', action='store_true')
    parser.add_argument('-c', '--compiler', default='gcc')
    parser.add_argument('-f', '--file', required=False, default=None)
    if len(sys.argv) < 2:
        parser.print_help()
        return
    if not sys.argv[1].startswith('-'):
        return cygtrace_call(sys.argv[1:])
    args = parser.parse_args()
    if args.demangle:
        if not args.file:
            raise ValueError('json file required')
        cygtrace_demangle(args.file)
    else:
        output = []
        if args.includes:
            output.append(get_includes())
        if args.libs:
            output.append(get_libs())
        if args.params:
            output.append(get_params(compiler=args.compiler, linking=(not args.module)))
        print(' '.join(output))
