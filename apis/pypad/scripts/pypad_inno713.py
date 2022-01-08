#!%PYTHON_BANGPATH%

# pypad_inno713.py
#
# Send Now & Next updates to an Innovonics 713 RDS encoder
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
import serial
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)


def ProcessPad(update):
    n=1
    section='Rds'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW):
            dps=''
            if(len(update.config().get(section,'DynamicPsString'))!=0):
                dps='DPS='+update.resolvePadFields(update.config().get(section,'DynamicPsString'),pypad.ESCAPE_NONE)+'\r\n'
            ps=''
            if(len(update.config().get(section,'PsString'))!=0):
                ps='PS='+update.resolvePadFields(update.config().get(section,'PsString'),pypad.ESCAPE_NONE)+'\r\n'
            text=''
            if(len(update.config().get(section,'RadiotextString'))!=0):
                text='TEXT='+update.resolvePadFields(update.config().get(section,'RadiotextString'),pypad.ESCAPE_NONE)+'\r\n'
            if(update.config().has_option(section,'Device')):
                #
                # Use serial output
                #
                tty_dev=update.config().get(section,'Device')
                speed=int(update.config().get(section,'Speed'))
                parity=serial.PARITY_NONE
                if int(update.config().get(section,'Parity'))==1:
                    parity=serial.PARITY_EVEN
                if int(update.config().get(section,'Parity'))==2:
                    parity=serial.PARITY_ODD
                bytesize=int(update.config().get(section,'WordSize'))
                dev=serial.Serial(tty_dev,speed,parity=parity,bytesize=bytesize)
                if(len(dps)!=0):
                    dev.write(dps.encode('utf-8'))
                if(len(ps)!=0):
                    dev.write(ps.encode('utf-8'))
                if(len(text)!=0):
                    dev.write(text.encode('utf-8'))
                dev.close()
            else:
                #
                # Use UDP output
                #
                ipaddr=update.config().get(section,'IpAddress')
                port=int(update.config().get(section,'UdpPort'))
                if(len(dps)!=0):
                    send_sock.sendto(dps.encode('utf-8'),(ipaddr,port))
                if(len(ps)!=0):
                    send_sock.sendto(ps.encode('utf-8'),(ipaddr,port))
                if(len(text)!=0):
                    send_sock.sendto(text.encode('utf-8'),(ipaddr,port))
        n=n+1
        section='Rds'+str(n)

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
    eprint('pypad_inno713.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
