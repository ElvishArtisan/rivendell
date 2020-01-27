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
import requests
import pypad
import string
import urllib
from io import BytesIO

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    if update.hasPadType(pypad.TYPE_NOW):
        for section in update.config().sections():
            try:
                version = update.config().get(section,'Version')
                update.syslog(syslog.LOG_INFO,'[PyPAD][%s] Version : %s' % (section,str(version)))
                song=update.resolvePadFields(update.config().get(section,'FormatString'),pypad.ESCAPE_NONE)
                url="http://%s:%s/admin.cgi" % (update.config().get(section,'Hostname'),str(update.config().get(section,'Tcpport')))
                headers = {'user-agent': 'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.2'}
                payload={'pass': update.escape(update.config().get(section,'Password'),pypad.ESCAPE_URL),
                         'mode': 'updinfo',
                         'song': song.strip(string.punctuation),
                         'sid': update.escape(update.config().get(section,'Sid'),pypad.ESCAPE_NONE)}

            except configparser.NoSectionError:
                return

            if update.shouldBeProcessed(section):
                try:
                    payload = urllib.parse.urlencode(payload, quote_via=urllib.parse.quote)
                    r = requests.get(url, params=payload, headers=headers)
                    update.syslog(syslog.LOG_INFO,'[PyPAD][%s] Update exit code: %s' % (section,str(r.status_code)))
                except requests.exceptions.RequestException as e:
                    update.syslog(syslog.LOG_WARNING,'[PyPAD][Shoutcast1] Update failed: ' + str(e))

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
