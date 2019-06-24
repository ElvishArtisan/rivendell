#!%PYTHON_BANGPATH%

# pypad_xcmd.py
#
# Send Now & Next updates to an RDS encoder supporting X-Command
#
#   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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
import syslog
import socket
import configparser
import serial
import xml.etree.ElementTree as ET
import time
import pypad

def eprint(*args,**kwargs):
    print(pypad_name+': ',file=sys.stderr,end='')
    print(*args,file=sys.stderr)

def XcmdResponse():
    resp_code={b'+':'Command processed successfully',b'!':'Unknown command',b'-':'Invalid argument',b'/':'Command processed partially'}

    while(True):
        try:
            response=send_sock.recv(1)
            if response==b'+': return True,response.decode('utf-8'),resp_code[response]
            if response==b'!': return True,response.decode('utf-8'),resp_code[response]
            if response==b'-': return True,response.decode('utf-8'),resp_code[response]
            if response==b'/': return True,response.decode('utf-8'),resp_code[response]

        except:
            return False,'','Exception'

def ProcessPad(update):
    n=1
    while(True):
        section='XCmd'+str(n)
        try:
            rds=ET.Element('rds')
            item=ET.SubElement(rds,'item')

            # Set destination code
            dest=ET.SubElement(item,'dest')
            try:
                dest.text=update.config().get(section,'DestCode')

            except configparser.NoOptionError:
                dest.text='7'

            if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW):
                radiotext=update.config().get(section,'RadioText')

                # Use pypad string in USER_DEFINED field if available
                try:
                    prefix=update.config().get(section,'UserDefinedPrefix').lower()
                    i=update.padField(pypad.TYPE_NOW,pypad.FIELD_USER_DEFINED).lower().find(prefix)
                    if i>-1:
                        radiotext=update.padField(pypad.TYPE_NOW,pypad.FIELD_USER_DEFINED)[i+len(prefix):].strip()

                except configparser.NoOptionError:
                    pass

                radiotext=radiotext.replace('%a','<artist>%a</artist>')
                radiotext=radiotext.replace('%t','<title>%t</title>')
                radiotext=radiotext.replace('%l','<album>%l</album>')

                radiotext=update.resolvePadFields(radiotext,pypad.ESCAPE_NONE)
            else:
                try:
                    radiotext=update.config().get(section,'DefaultText')

                except configparser.NoOptionError:
                    radiotext=''

            if '%1' in radiotext:
                if update.config().get(section,'StationNameShort'):
                    radiotext=radiotext.replace('%1','<short>'+update.config().get(section,'StationNameShort')+'</short>')

            if '%2' in radiotext:
                if update.config().get(section,'StationNameLong'):
                    radiotext=radiotext.replace('%2','<long>'+update.config().get(section,'StationNameLong')+'</long>')

            if '%3' in radiotext:
                if update.config().get(section,'URL'):
                    radiotext=radiotext.replace('%3','<page>'+update.config().get(section,'URL')+'</page>')

            if '%4' in radiotext:
                if update.config().get(section,'Phone'):
                    radiotext=radiotext.replace('%4','<phone>'+update.config().get(section,'Phone')+'</phone>')

            if '%5' in radiotext:
                if update.config().get(section,'SMS'):
                    radiotext=radiotext.replace('%5','<sms>'+update.config().get(section,'SMS')+'</sms>')

            if '%6' in radiotext:
                if update.config().get(section,'Email'):
                    radiotext=radiotext.replace('%6','<email>'+update.config().get(section,'Email')+'</email>')

            text=ET.SubElement(item,'text')
            text.text=radiotext

            xcmd=b'XCMD='+ET.tostring(rds)+b"\r"
            xcmd=xcmd.replace(b'&lt;',b'<')
            xcmd=xcmd.replace(b'&gt;',b'>')
            xcmd=xcmd.replace(b'&amp;',b'&')

            try:
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
                dev.write(xcmd.decode('utf-8'))
                dev.close()

            except configparser.NoOptionError:
                #
                # Create Send TCP Socket
                #
                send_sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                send_sock.settimeout(5)

                encoder=(update.config().get(section,'IpAddress'),int(update.config().get(section,'TcpPort')))
                update.syslog(syslog.LOG_INFO,'Connecting to {}:{}'.format(*encoder))
                update.syslog(syslog.LOG_INFO,xcmd.decode('utf-8'))

                try:
                    send_sock.connect(encoder)
                    send_sock.sendall(xcmd)
                    ack,response,respstr=XcmdResponse()
                    if response:
                        update.syslog(syslog.LOG_INFO,respstr)

                except OSError as e:
                    update.syslog(syslog.LOG_WARNING,"Socket error: {0}".format(e))

                except IOError as e:
                    errno,strerror=e.args
                    update.syslog(syslog.LOG_WARNING,"I/O error({0}): {1}".format(errno,strerror))

                update.syslog(syslog.LOG_INFO,'Closing connection')
                send_sock.close()

                # Give the device time to process and close before sending another command
                time.sleep(1)

            n=n+1
        except configparser.NoSectionError:
            return

#
# 'Main' function
#

#
# Program Name
#
pypad_name=sys.argv[0].split('/')[-1]

rcvr=pypad.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_xcmd.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
