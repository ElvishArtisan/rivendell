#!%PYTHON_BANGPATH%

# pypad_spinitron.py
#
# Write PAD updates to the Spinitron Playlist Service
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

def JsonField(update,tag,value,is_last=False):
    if (value==None) or (value==''):
        ret='  "'+tag+'": null'
    else:
        ret='  "'+tag+'": "'+update.escape(value,pypad.ESCAPE_JSON)+'"'
    if not is_last:
        ret+=','
    return ret+'\r\n'

def ProcessPad(update):
    try:
        last_updates[update.machine()]
    except KeyError:
        last_updates[update.machine()]=None

    n=1
    try:
        while(True):
            section='Spinitron'+str(n)
            if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW) and (last_updates[update.machine()] != update.startDateTimeString(pypad.TYPE_NOW)):
                last_updates[update.machine()]=update.startDateTimeString(pypad.TYPE_NOW)
                title=update.resolvePadFields(update.config().get(section,'Title'),pypad.ESCAPE_JSON)
                artist=update.resolvePadFields(update.config().get(section,'Artist'),pypad.ESCAPE_JSON)
                album=update.resolvePadFields(update.config().get(section,'Album'),pypad.ESCAPE_JSON)
                label=update.resolvePadFields(update.config().get(section,'Label'),pypad.ESCAPE_JSON)
                composer=update.resolvePadFields(update.config().get(section,'Composer'),pypad.ESCAPE_JSON)
                conductor=update.resolvePadFields(update.config().get(section,'Conductor'),pypad.ESCAPE_JSON)
                notes=update.resolvePadFields(update.config().get(section,'Notes'),pypad.ESCAPE_JSON)

                json='{\r\n'
                pmode=update.config().get(section,'PlaylistMode')
                if pmode=='Full':
                    json+='  "live": false\r\n'
                if pmode=='Assist':
                    json+='  "live:" true\r\n'
                if pmode=='Follow':
                    if update.mode()=='Automatic':
                        json+='  "live": false,\r\n'
                    else:
                        json+='  "live": true,\r\n'
                duration=str(update.padField(pypad.TYPE_NOW,pypad.FIELD_LENGTH)//1000)
                year=update.padField(pypad.TYPE_NOW,pypad.FIELD_YEAR)
                if year==None:
                    json+='  "released": null,\r\n'
                else:
                    json+='  "released": '+str(year)+',\r\n'
                json+='  "duration": '+duration+',\r\n'
                json+=JsonField(update,'artist',artist)
                json+=JsonField(update,'release',album)
                json+=JsonField(update,'label',label)
                json+=JsonField(update,'song',title)
                json+=JsonField(update,'composer',composer)
                json+=JsonField(update,'conductor',conductor)
                json+=JsonField(update,'note',notes)
                json+=JsonField(update,'isrc',update.padField(pypad.TYPE_NOW,pypad.FIELD_ISRC),True)
                json+='}\r\n'
                send_buf=BytesIO(json.encode('utf-8'))
                recv_buf=BytesIO()
                curl=pycurl.Curl()
                curl.setopt(curl.URL,'https://spinitron.com/api/spins')
                headers=[]
                headers.append('Authorization: Bearer '+update.config().get(section,'APIKey'))
                headers.append('Content-Type: application/json')
                headers.append('Content-Length: '+str(len(json.encode('utf-8'))))
                curl.setopt(curl.HTTPHEADER,headers);
                curl.setopt(curl.POST,True)
                curl.setopt(curl.READDATA,send_buf)
                curl.setopt(curl.WRITEDATA,recv_buf)
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
    eprint('pypad_spinitron.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
