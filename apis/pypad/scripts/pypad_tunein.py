#!%PYTHON_BANGPATH%

# pypad_tunein.py
#
# Send PAD updates to TuneIn
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

import os
import sys
import socket
import requests
import xml.etree.ElementTree as ET
import syslog
import pypad
import configparser

def ProcessPad(update):
    if update.hasPadType(pypad.TYPE_NOW):
        n=1
        while(True):
            section='Station'+str(n)
            try:
                values={}
                values['id']=update.config().get(section,'StationID')
                values['partnerId']=update.config().get(section,'PartnerID')
                values['partnerKey']=update.config().get(section,'PartnerKey')
                values['title']=update.resolvePadFields(update.config().get(section,'TitleString'),pypad.ESCAPE_NONE)
                values['artist']=update.resolvePadFields(update.config().get(section,'ArtistString'),pypad.ESCAPE_NONE)
                values['album']=update.resolvePadFields(update.config().get(section,'AlbumString'),pypad.ESCAPE_NONE)
                update.syslog(syslog.LOG_INFO,'Updating TuneIn: artist='+values['artist']+' title='+values['title']+' album='+values['album'])
                try:
                    response=requests.get('http://air.radiotime.com/Playing.ashx',params=values)
                    response.raise_for_status()
                except requests.exceptions.RequestException as e:
                    update.syslog(syslog.LOG_WARNING,str(e))
                else:
                    xml=ET.fromstring(response.text)
                    status=xml.find('./head/status')
                    if(status.text!='200'):
                        update.syslog(syslog.LOG_WARNING,'Update Failed: '+xml.find('./head/fault').text)
                n=n+1
            except configparser.NoSectionError:
                if(n==1):
                    update.syslog(syslog.LOG_WARNING,'No station config found')
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
    eprint('pypad_tunein.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
