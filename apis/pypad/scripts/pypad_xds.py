#!%PYTHON_BANGPATH%

# pypad_xds.py
#
# Send CICs via UDP or serial
#
#   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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
import time
import syslog
import pypad

#
# For supressing redundant 'now' updates
#
last_updates={}

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)


def FilterField(string):
    string=string.replace(' ','_')
    string=string.replace(',','_')
    string=string.replace('.','_')

    string=string.replace('"',"'")

    string=string.replace('%','-')
    string=string.replace('*','-')
    string=string.replace('+','-')
    string=string.replace('/','-')
    string=string.replace(':','-')
    string=string.replace(';','-')
    string=string.replace('<','-')
    string=string.replace('=','-')
    string=string.replace('>','-')
    string=string.replace('?','-')
    string=string.replace('@','-')
    string=string.replace('[','-')
    string=string.replace('\\','-')
    string=string.replace(']','-')
    string=string.replace('^','-')
    string=string.replace('{','-')
    string=string.replace('|','-')
    string=string.replace('}','-')

    return string


def ProcessPad(update):
    try:
        last_updates[update.machine()]
    except KeyError:
        last_updates[update.machine()]=None

    n=1
    section='Udp'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW) and update.hasService() and (last_updates[update.machine()] != update.startDateTimeString(pypad.TYPE_NOW)):
            last_updates[update.machine()]=update.startDateTimeString(pypad.TYPE_NOW)
            packet='0:'+update.serviceProgramCode()+':'+update.config().get(section,'IsciPrefix')+FilterField(update.padField(pypad.TYPE_NOW,pypad.FIELD_EXTERNAL_EVENT_ID))+':*'
            update.syslog(syslog.LOG_INFO,'sending CIC update "'+packet+'"')
            reps=range(1)
            if(update.config().has_option(section,'Repetitions')):
                reps=range(update.config().getint(section,'Repetitions'))
            delay=0.1
            if(update.config().has_option(section,'RepetitionDelay')):
                delay=update.config().getfloat(section,'RepetitionDelay')/1000.0
            for r in reps:
                try:
                    #
                    # Use serial output
                    #
                    tty_dev=update.config().get(section,'TtyDevice')
                    speed=int(update.config().get(section,'TtySpeed'))
                    parity=serial.PARITY_NONE
                    dev=serial.Serial(tty_dev,speed,parity=parity,bytesize=8)
                    dev.write(packet.encode('utf-8'))
                    dev.close()

                except configparser.NoOptionError:
                    #
                    # Use UDP output
                    #
                    send_sock.sendto(packet.encode('utf-8'),
                                     (update.config().get(section,'IpAddress'),int(update.config().get(section,'UdpPort'))))
                time.sleep(delay)
        n=n+1
        section='Udp'+str(n)


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
    eprint('pypad_xds.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
