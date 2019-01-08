#!%PYTHON_BANGPATH%

# pypad_spottrap.py
#
# Output Now & Next data on the basis of Group and Length.
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

import sys
import socket
import configparser
import pypad

last_updates={}

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)


def ProcessPad(update):
    try:
        last_updates[update.machine()]
    except KeyError:
        last_updates[update.machine()]=None

    n=1
    while(True):
        section='Rule'+str(n)
        try:
            if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW) and (last_updates[update.machine()] != update.startDateTimeString(pypad.TYPE_NOW)):
                last_updates[update.machine()]=update.startDateTimeString(pypad.TYPE_NOW)
                length=update.padField(pypad.TYPE_NOW,pypad.FIELD_LENGTH)
                if update.padField(pypad.TYPE_NOW,pypad.FIELD_GROUP_NAME)==update.config().get(section,'GroupName') and length>=int(update.config().get(section,'MinimumLength')) and length<=int(update.config().get(section,'MaximumLength')):
                    msg=update.resolvePadFields(update.config().get(section,'FormatString'),pypad.ESCAPE_NONE)
                else:
                    msg=update.resolvePadFields(update.config().get(section,'DefaultFormatString'),pypad.ESCAPE_NONE)
                send_sock.sendto(msg.encode('utf-8'),
                                 (update.config().get(section,'IpAddress'),int(update.config().get(section,'UdpPort'))))
            n=n+1
        except configparser.NoSectionError:
            return

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
    eprint('pypad_spottrap.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
