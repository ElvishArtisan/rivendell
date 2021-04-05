#!/usr/bin/python3

# pypad_ino713_tcp.py
#
# Send Now & Next updates to an Inovonics 713 RDS encoder as TCP
#
#   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
#                 2020 Eric Adler <eric@whrwfm.org>
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
import time

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
            #
            # Use TCP output
            #
            waittime=int(update.config().get(section,'Delay'))
            time.sleep(waittime)
            ipaddr=update.config().get(section,'IpAddress')
            port=int(update.config().get(section,'TcpPort'))
            if(len(dps)!=0):
                send_sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                send_sock.connect((ipaddr,port))
                send_sock.sendall(dps.encode('utf-8'))
                send_sock.close()
            if(len(ps)!=0):
                send_sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                send_sock.connect((ipaddr,port))
                send_sock.sendall(ps.encode('utf-8'))
                send_sock.close()
            if(len(text)!=0):
                send_sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                send_sock.connect((ipaddr,port))
                send_sock.sendall(text.encode('utf-8'))
                send_sock.close()
        n=n+1
        section='Rds'+str(n)

#
# 'Main' function
#
# Create Send Socket
#

rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_inno713.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
