#!%PYTHON_BANGPATH%

# pypad_icecast2.py
#
# Send PAD updates to Icecast2 mountpoint
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

import os.path
import sys
import socket
import requests
from requests.auth import HTTPBasicAuth
import xml.etree.ElementTree as ET
import syslog
import pypad
import configparser

def ProcessPad(update):
    if update.hasPadType(pypad.TYPE_NOW):
        n=1
        while(True):
            section='Icecast'+str(n)
            try:
                values={}
                values['mount']=update.config().get(section,'Mountpoint')
                values['song']=update.resolvePadFields(update.config().get(section,'FormatString'),pypad.ESCAPE_NONE)
                values['mode']='updinfo'
                update.syslog(syslog.LOG_INFO,'Updating '+update.config().get(section,'Hostname')+': song='+values['song'])
                url="http://%s:%s/admin/metadata" % (update.config().get(section,'Hostname'),update.config().get(section,'Tcpport'))
                try:
                    response=requests.get(url,auth=HTTPBasicAuth(update.config().get(section,'Username'),update.config().get(section,'Password')),params=values)
                    response.raise_for_status()
                except requests.exceptions.RequestException as e:
                    update.syslog(syslog.LOG_WARNING,str(e))
                n=n+1
            except configparser.NoSectionError:
                if(n==1):
                    update.syslog(syslog.LOG_WARNING,'No icecast config found')
                return

#
# Program Name
#
pypad_name=sys.argv[0].split('/')[-1]

#
# Create Send Socket
#
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

#
# Start Receiver
#
rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_icecast2: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
