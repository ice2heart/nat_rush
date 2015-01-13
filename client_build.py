#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import platform
import re
import subprocess
import sys
import shutil
from string import Template

def run_process(*args, **kwargs):
    shell = kwargs.get('shell', False)
    stdout = subprocess.PIPE if kwargs.get('return_output', False) else None
    p = subprocess.Popen(args, stdout=stdout, shell=shell)
    ret = p.communicate()[0]
    if p.returncode != 0:
        raise ValueError(*args)
    return ret

def generate_from_template(template_file, result_file, cur_dict):
    with open(template_file, 'r') as cur_template:
        with open(result_file, 'w') as write_file:
            for line in cur_template.xreadlines():
                write_file.write(Template(line).substitute(cur_dict))
            st = os.stat(template_file)
            os.chmod(result_file, st.st_mode)

def read_file_to_dict(file_name):
    p = re.compile(r'^([^=]+)=([^=]+)$')
    out = dict()
    with open(file_name, 'r') as  cur_dict_file:
        for line in cur_dict_file.xreadlines():
            for iter_group in p.finditer(line):
                out[iter_group.group(1)] = iter_group.group(2).strip()
    return out

def mkpath(path):
    if not os.path.isdir(path):
        os.makedirs(path)

def copytree(src, dst, recursive=False, ignore=None):
    names = os.listdir(src)
    if ignore is not None:
        ignored_names = ignore(src, names)
    else:
        ignored_names = []
    mkpath(dst)
    errors = []
    for name in names:
        if not recursive and name in ignored_names:
            continue
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if os.path.isdir(srcname):
                copytree(srcname, dstname, ignore)
            else:
                # Will raise a SpecialFileError for unsupported file types
                if name in ignored_names:
                    continue
                shutil.copy2(srcname, dstname)
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except shutil.Error as err:
            errors.extend(err.args[0])
        except EnvironmentError, why:
            errors.append((srcname, dstname, str(why)))
    try:
        shutil.copystat(src, dst)
    except OSError, why:
        if WindowsError is not None and isinstance(why, WindowsError):
            # Copying file access times may fail on Windows
            pass
        else:
            errors.extend((src, dst, str(why)))
    if errors:
        raise shutil.Error, errors
def rmtree(filename):
    if not os.path.isdir(filename):
        return
    def handleRemoveReadonly(func, path, exc):
        excvalue = exc[1]
        if func in (os.rmdir, os.remove) and excvalue.errno == errno.EACCES:
            os.chmod(path, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)  # 0777
            func(path)
        else:
            raise
    shutil.rmtree(filename, ignore_errors=False, onerror=handleRemoveReadonly)


is_windows = False
is_macos = False
is_linux = False

QMAKESPEC = None
QADDITIONALPARAMS = ['CONFIG+=Release']

if platform.system() == 'Windows':
    QMAKESPEC = 'win32-msvc2010'
    QADDITIONALPARAMS = ['-tp', 'vc'] + QADDITIONALPARAMS
    is_windows = True
elif platform.system() == 'Darwin':
    #QMAKESPEC = 'macx-g++'
    # YEAH we use icc under MAC OS, lol
    #QADDITIONALPARAMS = ['-spec', 'macx-icc'] + QADDITIONALPARAMS
    is_macos = True
else:
    is_linux = True
conf_dict = None
if os.path.exists('preference.conf'):
    conf_dict = read_file_to_dict('preference.conf')
else:
    print 'need preference.conf'
    exit(0)

QT_BIN = conf_dict['QT_BIN']

QMAKE_PATH = QT_BIN + '/qmake'

QT_LIB_PATH = run_process(QMAKE_PATH, '-query', 'QT_INSTALL_LIBS', return_output=True).strip()
QT_PLUGINS_PATH = run_process(QMAKE_PATH, '-query', 'QT_INSTALL_PLUGINS', return_output=True).strip()


PROJECTSDIR = os.path.abspath('.')
CLIENTDIR = os.path.join(PROJECTSDIR, 'client')
TEMPATE_DIR = os.path.join(PROJECTSDIR, 'templates')
def make_template_file():
    generate_from_template(os.path.join(TEMPATE_DIR, 'config.ini.templ'), os.path.join(CLIENTDIR, 'config.ini'), conf_dict)

def make_client():
    os.chdir(CLIENTDIR)
    run_process(QMAKE_PATH, 'client.pro', *QADDITIONALPARAMS)
    #clear after macdeployqt
    if os.path.isdir(os.path.join(CLIENTDIR, 'client.app')):
        rmtree('client.app')
    if is_windows:
        run_process('msbuild', 'client.vcxproj', '/p:UseEnv=true;')
    else:
        run_process('make', 'clean')
        run_process('make')

def make_package():
    os.chdir(CLIENTDIR)
    if is_macos:
        rmtree('dmg')
        run_process(os.path.join(QT_BIN, 'macdeployqt'), 'client.app')
        shutil.copy2('config.ini', os.path.join('client.app', 'Contents', 'MacOS'))
        mkpath(os.path.join(CLIENTDIR, 'dmg'))
        mkpath(os.path.join(CLIENTDIR, 'dmg', 'client.app'))
        copytree('client.app', os.path.join('dmg', 'client.app'))
        run_process('hdiutil', 'create', '-ov', '-srcfolder', 'dmg/', '-format', 'UDRW', '-volname',  'client.dmg', 'client.dmg')


if __name__ == '__main__':
    make_template_file()
    make_client()
    make_package()


