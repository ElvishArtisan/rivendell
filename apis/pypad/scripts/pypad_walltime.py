#!%PYTHON_BANGPATH%

# pypad_walltime.py
#
# Write PAD updates a WallTime text widget.
#
#   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import sys
import syslog
import configparser
import pycurl
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style
from io import BytesIO

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    n=1
    section='Walltime'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section):
            fmtstr=update.config().get(section,'FormatString')
            buf=BytesIO(update.resolvePadFields(fmtstr,pypad.ESCAPE_NONE).encode('utf-8'))
            curl=pycurl.Curl()
            curl.setopt(curl.URL,'http://'+update.config().get(section,'IpAddress')+'/webwidget');
            curl.setopt(curl.USERNAME,'user')
            curl.setopt(curl.PASSWORD,update.config().get(section,'Password'))
            curl.setopt(curl.UPLOAD,True)
            curl.setopt(curl.READDATA,buf)
            try:
                curl.perform()
            except pycurl.error:
                update.syslog(syslog.LOG_WARNING,'['+section+'] failed: '+curl.errstr())
            curl.close()
        n=n+1
        section='Walltime'+str(n)


#
# 'Main' function
#
rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_walltime.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
