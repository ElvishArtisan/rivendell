#!%PYTHON_BANGPATH%

# pypad_shoutcast1.py
#
# Write PAD updates to a Shoutcast 1 instance
#
#   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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
import pypad
from io import BytesIO

last_updates={}

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    try:
        last_updates[update.machine()]
    except KeyError:
        last_updates[update.machine()]=None

    n=1
    try:
        while(True):
            section='Shoutcast'+str(n)
            if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW) and (last_updates[update.machine()] != update.startDateTimeString(pypad.TYPE_NOW)):
                last_updates[update.machine()]=update.startDateTimeString(pypad.TYPE_NOW)
                song=update.resolvePadFields(update.config().get(section,'FormatString'),pypad.ESCAPE_URL)
                url='http://'+update.config().get(section,'Hostname')+':'+str(update.config().get(section,'Tcpport'))+'/admin.cgi?pass='+update.escape(update.config().get(section,'Password'),pypad.ESCAPE_URL)+'&mode=updinfo&song='+song
                curl=pycurl.Curl()
                curl.setopt(curl.URL,url)
                headers=[]
                #
                # D.N.A.S v1.9.8 refuses to process updates with the default
                # CURL user-agent value, hence we lie to it.
                #
                headers.append('User-Agent: '+'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.2')
                curl.setopt(curl.HTTPHEADER,headers);
                try:
                    curl.perform()
                    code=curl.getinfo(pycurl.RESPONSE_CODE)
                    if (code<200) or (code>=300):
                        update.syslog(syslog.LOG_WARNING,'['+section+'] returned response code '+str(code))
                except pycurl.error:
                    update.syslog(syslog.LOG_WARNING,'['+section+'] failed: '+curl.errstr())
                curl.close()
            n=n+1

    except configparser.NoSectionError:
        return

#
# 'Main' function
#
rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_shoutcast1.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
