#!%PYTHON_BANGPATH%

# pypad_xmpad.py
#
# Write PAD updates to a Sirius/XM channel
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
import syslog
import configparser
import serial
import PyPAD

XMPAD_DELIMITER='\02'
# XMPAD_DELIMITER='\x7c' #(|)

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)


def MakeA4(update,section):
    a4='A4'

    a4+=XMPAD_DELIMITER+'1'

    field1=update.resolvePadFields(update.config().get(section,'FormatString1'),
                                   PyPAD.ESCAPE_NONE)
    a4+=XMPAD_DELIMITER+field1[0:16]

    a4+='\n'

    return a4

def MakeA5(update,section):
    a5='A5'

    a5+=XMPAD_DELIMITER+'1'

    field2=update.resolvePadFields(update.config().get(section,'FormatString2'),
                                   PyPAD.ESCAPE_NONE)
    a5+=XMPAD_DELIMITER+field2[0:16]

    a5+='\n'

    return a5


def MakeB4(update,section):
    b4='B-4'

    #
    # Recording flag
    #
    recording='1'
    if update.config().get(section,'Recording').lower()=='yes':
        recording='0'
    b4+=XMPAD_DELIMITER+recording

    b4+=XMPAD_DELIMITER+'1'

    #
    # Event duration
    #
    b4+=XMPAD_DELIMITER+str(update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_LENGTH)//432)

    b4+=XMPAD_DELIMITER+'0'

    #
    # Display field sizes
    #
    disp1size=update.config().get(section,'DisplaySize1')
    mask1=''
    for i in range(int(disp1size)):
        mask1+='1'
    disp2size=update.config().get(section,'DisplaySize2')
    mask2=''
    for i in range(int(disp2size)):
        mask2+='1'
    disp2size=update.config().get(section,'DisplaySize2')
    b4+=XMPAD_DELIMITER+disp1size
    b4+=XMPAD_DELIMITER+disp2size
    b4+=XMPAD_DELIMITER+mask1
    b4+=XMPAD_DELIMITER+mask2

    #
    # Display field values
    #
    field1=update.resolvePadFields(update.config().get(section,'FormatString1'),
                                   PyPAD.ESCAPE_NONE)
    field2=update.resolvePadFields(update.config().get(section,'FormatString2'),
                                   PyPAD.ESCAPE_NONE)
    b4+=XMPAD_DELIMITER+field2[0:16]
    b4+=XMPAD_DELIMITER+mask1
    b4+=XMPAD_DELIMITER+mask2
    b4+=XMPAD_DELIMITER+field1[0:16]

    #
    # Program ID
    #
    b4+=XMPAD_DELIMITER+update.config().get(section,'ProgramID')

    #
    # EOM
    #
    b4+='\n'

    return b4


def OpenSerialDevice(config,section):
    devname=config.get(section,'Device')
    speed=int(config.get(section,'Speed'))
    parity=serial.PARITY_NONE
    if int(config.get(section,'Parity'))==1:
        parity=serial.PARITY_EVEN
    if int(config.get(section,'Parity'))==2:
        parity=serial.PARITY_ODD
    bytesize=int(config.get(section,'WordSize'))
    return serial.Serial(devname,speed,parity=parity,bytesize=bytesize)


def ProcessTimer(config):
    n=1
    try:
        while(True):
            section='Serial'+str(n)
            dev=OpenSerialDevice(config,section)
            dev.write('H0\n'.encode('utf-8'))
            dev.close()
            n=n+1

    except configparser.NoSectionError:
        return

def ProcessPad(update):
    n=1
    try:
        while(True):
            section='Serial'+str(n)
            if update.shouldBeProcessed(section) and update.hasPadType(PyPAD.TYPE_NOW):
                dev=OpenSerialDevice(update.config(),section)
                b4=MakeB4(update,section)
                a4=MakeA4(update,section)
                a5=MakeA5(update,section)
                dev.write(b4.encode('utf-8'))
                dev.write(b4.encode('utf-8'))
                dev.write(b4.encode('utf-8'))
                dev.write(a4.encode('utf-8'))
                dev.write(a5.encode('utf-8'))
                dev.close()
            n=n+1

    except configparser.NoSectionError:
        return

#
# 'Main' function
#
syslog.openlog(sys.argv[0].split('/')[-1])

rcvr=PyPAD.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_xmpad.py: you must specify a configuration file')
    sys.exit(1)
rcvr.setCallback(ProcessPad)
rcvr.setTimerCallback(30,ProcessTimer)
rcvr.start(sys.argv[1],int(sys.argv[2]))
