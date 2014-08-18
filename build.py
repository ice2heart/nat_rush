#!/usr/bin/env python
# need python-pip, sh (python lib "pip install sh"), qmake, qt4-dev
# 
import sh
sh.git.clone('https://github.com/ice2heart/nat_rush')
sh.cd('nat_rush')
sh.cd('server')
sh.cd('QtWebsocket')
sh.qmake('QtWebsocket.pro')
sh.make()
sh.cd('..')
sh.qmake('server.pro')
sh.make()
