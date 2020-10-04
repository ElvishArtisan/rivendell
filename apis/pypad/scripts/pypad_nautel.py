#!/usr/bin/python3

# pypad_nautel.py
#
# Send Now & Next updates to an Nautel FM Transmitter as TCP for RDS
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


def sendvar(var):
    if(len(var)!=0):
        send_sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        send_sock.connect((ipaddr,port))
        send_sock.sendall(var.encode('utf-8'))
        send_sock.close()

def getval(val, update, section):
    try:
        configval = update.config().get(section,val)
    except:
        configval = ''
    return configval

def encode(srcdata, fieldname, update):
    if(len(srcdata)!=0):
        enc=fieldname+'='+update.resolvePadFields(srcdata,pypad.ESCAPE_NONE)+'\r\n'
    else:
        enc=''
    return enc

def getval_encode(sourcename, fieldname, update, section):
    return encode(getval(sourcename, update, section), fieldname, update)

def ProcessPad(update):
    n=1
    section='Rds'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section) and update.hasPadType(pypad.TYPE_NOW):
            dps=''
            dps=getval_encode('DynamicPS',"DPS",update,section)
            ps=getval_encode('ProgramService',"PS",update,section)
            text=getval_encode('RadioText',"TEXT",update,section)
            picode=getval_encode('PICode',"PI",update,section)
            pty=getval_encode('ProgramType',"PTY",update,section)
            ptyn=getval_encode('ProgramTypeName',"PTYN",update,section)
            trp=getval_encode('TrafficProgram',"TP",update,section)
            tra=getval_encode('TrafficAnnouncement',"TA",update,section)
            af1=getval_encode('AltFreq1',"AF1",update,section)
            af2=getval_encode('AltFreq2',"AF2",update,section)
            af3=getval_encode('AltFreq3',"AF3",update,section)
            af4=getval_encode('AltFreq4',"AF4",update,section)
            af5=getval_encode('AltFreq5',"AF5",update,section)
            af6=getval_encode('AltFreq6',"AF6",update,section)
            af7=getval_encode('AltFreq7',"AF7",update,section)
            af8=getval_encode('AltFreq8',"AF8",update,section)
            af9=getval_encode('AltFreq9',"AF9",update,section)
            af10=getval_encode('AltFreq10',"AF10",update,section)
            af11=getval_encode('AltFreq11',"AF11",update,section)
            af12=getval_encode('AltFreq12',"AF12",update,section)
            af13=getval_encode('AltFreq13',"AF13",update,section)
            af14=getval_encode('AltFreq14',"AF14",update,section)
            af15=getval_encode('AltFreq15',"AF15",update,section)
            af16=getval_encode('AltFreq16',"AF16",update,section)
            af17=getval_encode('AltFreq17',"AF17",update,section)
            af18=getval_encode('AltFreq18',"AF18",update,section)
            af19=getval_encode('AltFreq19',"AF19",update,section)
            af20=getval_encode('AltFreq20',"AF20",update,section)
            af21=getval_encode('AltFreq21',"AF21",update,section)
            af22=getval_encode('AltFreq22',"AF22",update,section)
            af23=getval_encode('AltFreq23',"AF23",update,section)
            af24=getval_encode('AltFreq24',"AF24",update,section)
            af25=getval_encode('AltFreq25',"AF25",update,section)
            di=getval_encode('DecoderInfo',"DI",update,section)
            mus=getval_encode('MusicSpeech',"MS",update,section)
            dat=getval_encode('Date',"DATE",update,section)
            tim=getval_encode('Time',"DATE",update,section)
            utco=getval_encode('UTCOffset',"UTC",update,section)
            cont=getval_encode('Cont',"CT",update,section)
            dpsr=getval_encode('DPSRate',"DPSR",update,section)
            dpsm=getval_encode('DPSMode',"DPSM",update,section)



            #
            # Use TCP output
            #
            waittime=int(update.config().get(section,'Delay'))
            time.sleep(waittime)
            ipaddr=update.config().get(section,'IpAddress')
            port=int(update.config().get(section,'TcpPort'))
            sendvar(dps)
            sendvar(ps)
            sendvar(text)
            
            sendvar(picode)
            sendvar(pty)
            sendvar(ptyn)
            sendvar(trp)
            sendvar(tra)
            sendvar(af1)
            sendvar(af2)
            sendvar(af3)
            sendvar(af4)
            sendvar(af5)
            sendvar(af6)
            sendvar(af7)
            sendvar(af8)
            sendvar(af9)
            sendvar(af10)
            sendvar(af11)
            sendvar(af12)
            sendvar(af13)
            sendvar(af14)
            sendvar(af15)
            sendvar(af16)
            sendvar(af17)
            sendvar(af18)
            sendvar(af19)
            sendvar(af20)
            sendvar(af21)
            sendvar(af22)
            sendvar(af23)
            sendvar(af24)
            sendvar(af25)
            sendvar(di)
            sendvar(mus)
            sendvar(dat)
            sendvar(tim)
            sendvar(utco)
            sendvar(cont)
            sendvar(dpsr)
            sendvar(dpsm)


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
