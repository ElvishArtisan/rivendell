#!%PYTHON_BANGPATH%

# pypad_serial.py
#
# Write PAD updates to serial (tty) devices.
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
import pypad
import serial
from io import BytesIO

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    n=1
    section='Serial'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section):
            devname=update.config().get(section,'Device')
            speed=int(update.config().get(section,'Speed'))
            parity=serial.PARITY_NONE
            if int(update.config().get(section,'Parity'))==1:
                parity=serial.PARITY_EVEN
            if int(update.config().get(section,'Parity'))==2:
                parity=serial.PARITY_ODD
            bytesize=int(update.config().get(section,'WordSize'))
            dev=serial.Serial(devname,speed,parity=parity,bytesize=bytesize)
            fmtstr=update.config().get(section,'FormatString')
            esc=int(update.config().get(section,'Encoding'))
            dev.write(update.resolvePadFields(fmtstr,esc).encode('utf-8'))
            dev.close()
        n=n+1
        section='Serial'+str(n)


#
# 'Main' function
#
rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_serial.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
