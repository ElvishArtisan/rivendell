#!/usr/bin/python

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

from __future__ import print_function

import os
import sys
import socket
import urllib
import xml.etree.ElementTree as ET
import syslog
import PyPAD
try:
    import configparser
except ImportError:
    import ConfigParser as configparser

def eprint(*args,**kwargs):
    print(pypad_name+': ',file=sys.stderr,end='',**kwargs)
    print(*args,file=sys.stderr,**kwargs)
    syslog.syslog(syslog.LOG_ERR,*args)

def iprint(*args,**kwargs):
    print(pypad_name+': ',file=sys.stdout,end='',**kwargs)
    print(*args,sep=pypad_name+': ',file=sys.stdout,**kwargs)
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
                values['id']=config.get(section,'StationID')
                values['partnerId']=config.get(section,'PartnerID')
                values['partnerKey']=config.get(section,'PartnerKey')
                values['title']=update.resolvePadFields(config.get(section,'TitleString'),PyPAD.ESCAPE_URL)
                values['artist']=update.resolvePadFields(config.get(section,'ArtistString'),PyPAD.ESCAPE_URL)
                values['album']=update.resolvePadFields(config.get(section,'AlbumString'),PyPAD.ESCAPE_URL)
                url_values=urllib.urlencode(values)
                url='http://air.radiotime.com/Playing.ashx?'+url_values
                iprint('Updating TuneIn: artist='+values['artist']+' title='+values['title']+' album='+values['album'])
                try:
                    response=urllib.urlopen(url)
                except:
                    eprint(url+' code='+str(response.getcode()))
                else:
                    xml=ET.fromstring(response.read())
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
syslog.openlog(logoption=syslog.LOG_PID, facility=syslog.LOG_DAEMON)

#
# Read Configuration
#
if len(sys.argv)>=2:
    fp=open(sys.argv[1])
    config=configparser.ConfigParser()
    config.readfp(fp)
    fp.close()
else:
    eprint('You must specify a configuration file')
    sys.exit(1)

#
# Daemonize
#
try:
    daemon=config.get(section,'Daemonize')
except:
    daemon='0'

if isTrue(daemon):
    try:
        pid=os.fork()
    except:
        eprint('Could not fork')
        sys.exit(1)
        
    if (pid==0):
        os.setsid()
    else:
        exit(0)

#
# Create Send Socket
#
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

#
# Start Receiver
#
rcvr=PyPAD.Receiver()
rcvr.setCallback(ProcessPad)
iprint('Started')
rcvr.start("localhost",PyPAD.PAD_TCP_PORT)
iprint('Stopped')
