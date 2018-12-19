#!%PYTHON_BANGPATH%

# pypad_tunein.py
#
# Send PAD updates to TuneIn
#
#   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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
import PyPAD
import configparser

def eprint(*args,**kwargs):
    print(pypad_name+': ',file=sys.stderr,end='')
    print(*args,file=sys.stderr)
    syslog.syslog(syslog.LOG_ERR,*args)

def iprint(*args,**kwargs):
    print(pypad_name+': ',file=sys.stdout,end='')
    print(*args,file=sys.stdout)
    syslog.syslog(syslog.LOG_INFO,*args)

def isTrue(string):
    l=['Yes','On','True','1']
    return string.lower() in map(str.lower,l)

def ProcessPad(update):
    if update.hasPadType(PyPAD.TYPE_NOW):
        n=1
        while(True):
            section='Station'+str(n)
            try:
                values={}
                values['id']=update.config().get(section,'StationID')
                values['partnerId']=update.config().get(section,'PartnerID')
                values['partnerKey']=update.config().get(section,'PartnerKey')
                values['title']=update.resolvePadFields(update.config().get(section,'TitleString'),PyPAD.ESCAPE_URL)
                values['artist']=update.resolvePadFields(update.config().get(section,'ArtistString'),PyPAD.ESCAPE_URL)
                values['album']=update.resolvePadFields(update.config().get(section,'AlbumString'),PyPAD.ESCAPE_URL)
                iprint('Updating TuneIn: artist='+values['artist']+' title='+values['title']+' album='+values['album'])
                try:
                    response=requests.get('http://air.radiotime.com/Playing.ashx',params=values)
                    response.raise_for_status()
                except requests.exceptions.RequestException as e:
                    eprint(str(e))
                else:
                    xml=ET.fromstring(response.text)
                    status=xml.find('./head/status')
                    if(status.text!='200'):
                        eprint('Update Failed: '+xml.find('./head/fault').text)
                n=n+1
            except configparser.NoSectionError:
                if(n==1):
                    eprint('No station config found')
                return

#
# Program Name
#
pypad_name=os.path.basename(__file__)

#
# Open Syslog
#
syslog.openlog(pypad_name,logoption=syslog.LOG_PID,facility=syslog.LOG_DAEMON)


#
# Create Send Socket
#
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

#
# Start Receiver
#
rcvr=PyPAD.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('You must specify a configuration file')
    sys.exit(1)
rcvr.setCallback(ProcessPad)
iprint('Started')
rcvr.start(sys.argv[1],int(sys.argv[2]))
iprint('Stopped')