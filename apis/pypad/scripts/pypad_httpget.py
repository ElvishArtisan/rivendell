#!%PYTHON_BANGPATH%

# pypad_httpget.py
#
# Write PAD updates to HTTP GET URL
#
#   (C) Copyright 2019-2022 Fred Gleason <fredg@paravelsystems.com>
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
import configparser
import pycurl
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style
import syslog
from io import BytesIO

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    if update.hasPadType(pypad.TYPE_NOW):
        n=1
        while(True):
            section='Url'+str(n)
            try:
                username=''
                if update.config().has_option(section,'Username'):
                    username=update.config().get(section,'Username')
                password=''
                if update.config().has_option(section,'Password'):
                    password=update.config().get(section,'Password')
                url=update.resolvePadFields(update.config().get(section,'URL'),pypad.ESCAPE_URL)
                buf=BytesIO()
                curl=pycurl.Curl()
                curl.setopt(curl.URL,url)
                curl.setopt(curl.WRITEDATA,buf)
                curl.setopt(curl.FOLLOWLOCATION,True)
                if (username!=""):
                    curl.setopt(curl.USERNAME,username)
                if (password!=""):
                    curl.setopt(curl.PASSWORD,password)

            except configparser.NoSectionError:
                if(n==1):
                    update.syslog(syslog.LOG_WARNING,'No httpget config found')
                return

            if update.shouldBeProcessed(section):
                try:
                    curl.perform()
                    update.syslog(syslog.LOG_DEBUG,'['+section+'] sending URL "'+url+'"')
                    code=curl.getinfo(pycurl.RESPONSE_CODE)
                    if (code<200) or (code>=300):
                        update.syslog(syslog.LOG_WARNING,'['+section+'] returned response code '+str(code))
                    else:
                        update.syslog(syslog.LOG_NOTICE,'['+section+'] successful')

                except pycurl.error:
                    update.syslog(syslog.LOG_WARNING,'['+section+'] failed: '+curl.errstr())
                    update.syslog(syslog.LOG_WARNING,'['+section+'] URL: '+url)

                curl.close()

            n=n+1

#
# 'Main' function
#
rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_httpget.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
