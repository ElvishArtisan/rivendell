#!%PYTHON_BANGPATH%

# pypad_ando.py
#
# Send PAD updates to an Ando AdInjector
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
import socket
import configparser
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style

last_updates={}

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessTimer(config):
    n=1
    section='System'+str(n)
    while(config.has_section(section)):
        send_sock.sendto('HB'.encode('utf-8'),
                         (config.get(section,'IpAddress'),int(config.get(section,'UdpPort'))))
        n=n+1
        section='System'+str(n)
    return

def ProcessPad(update):
    try:
        last_updates[update.machine()]
    except KeyError:
        last_updates[update.machine()]=None

    n=1
    section='System'+str(n)
    while(update.config().has_section(section)):
        section='System'+str(n)
        if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW) and (last_updates[update.machine()] != update.startDateTimeString(pypad.TYPE_NOW)):
            last_updates[update.machine()]=update.startDateTimeString(pypad.TYPE_NOW)
            title=update.resolvePadFields(update.config().get(section,'Title'),pypad.ESCAPE_NONE)
            artist=update.resolvePadFields(update.config().get(section,'Artist'),pypad.ESCAPE_NONE)
            album=update.resolvePadFields(update.config().get(section,'Album'),pypad.ESCAPE_NONE)
            label=update.resolvePadFields(update.config().get(section,'Label'),pypad.ESCAPE_NONE)
            secs=update.padField(pypad.TYPE_NOW,pypad.FIELD_LENGTH)
            duration=('%02d:' % (secs//60000))+('%02d' % ((secs%60000)//1000))
            group=update.padField(pypad.TYPE_NOW,pypad.FIELD_GROUP_NAME)
            if update.config().get(section,'Label')=='':
                msg='^'+artist+'~'+title+'~'+duration+'~'+group+'~'+album+'~'+str(update.padField(pypad.TYPE_NOW,pypad.FIELD_CART_NUMBER))+'|'
            else:
                msg='^'+artist+'~'+title+'~'+duration+'~'+group+'~'+str(update.padField(pypad.TYPE_NOW,pypad.FIELD_CART_NUMBER))+'~'+album+'~'+label+'|'
            send_sock.sendto(msg.encode('utf-8'),
                             (update.config().get(section,'IpAddress'),int(update.config().get(section,'UdpPort'))))
        n=n+1

#
# 'Main' function
#
# Create Send Socket
#
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_ando.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.setTimerCallback(30,ProcessTimer)
rcvr.start(sys.argv[1],int(sys.argv[2]))
