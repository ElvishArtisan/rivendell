#!%PYTHON_BANGPATH%

# pypad_udp.py
#
# Send PAD updates via UDP
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

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    n=1
    while(True):
        section='Udp'+str(n)
        try:
            if update.shouldBeProcessed(section):
                fmtstr=update.config().get(section,'FormatString')
                send_sock.sendto(update.resolvePadFields(fmtstr,int(update.config().get(section,'Encoding'))).encode('utf-8'),
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
    eprint('pypad_udp.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
