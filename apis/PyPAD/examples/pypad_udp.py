#%PYTHON_BANGPATH%

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
import PyPAD

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def processUpdate(update,section):
    if config.get(section,'ProcessNullUpdates')=='0':
        return True
    if config.get(section,'ProcessNullUpdates')=='1':
        return update.hasPadType(PyPAD.TYPE_NOW)
    if config.get(section,'ProcessNullUpdates')=='2':
        return update.hasPadType(PyPAD.TYPE_NEXT)
    if config.get(section,'ProcessNullUpdates')=='3':
        return update.hasPadType(PyPAD.TYPE_NOW) and update.hasPadType(PyPAD.TYPE_NEXT)

    log_dict={1: 'MasterLog',2: 'Aux1Log',3: 'Aux2Log',
              101: 'VLog101',102: 'VLog102',103: 'VLog103',104: 'VLog104',
              105: 'VLog105',106: 'VLog106',107: 'VLog107',108: 'VLog108',
              109: 'VLog109',110: 'VLog110',111: 'VLog111',112: 'VLog112',
              113: 'VLog113',114: 'VLog114',115: 'VLog115',116: 'VLog116',
              117: 'VLog117',118: 'VLog118',119: 'VLog119',120: 'VLog120'}
    if config.get(section,log_dict[update.machine()]).lower()=='yes':
        return True
    if config.get(section,log_dict[update.machine()]).lower()=='no':
        return False
    if config.get(section,log_dict[update.machine()]).lower()=='onair':
        return update.onairFlag()

def ProcessPad(update):
    n=1
    while(True):
        section='Udp'+str(n)
        try:
            if processUpdate(update,section):
                fmtstr=config.get(section,'FormatString')
                send_sock.sendto(update.resolvePadFields(fmtstr,int(config.get(section,'Encoding'))).encode('utf-8'),
                                 (config.get(section,'IpAddress'),int(config.get(section,'UdpPort'))))
            n=n+1
        except configparser.NoSectionError:
            return

#
# Read Configuration
#
if len(sys.argv)>=2:
    fp=open(sys.argv[1])
    config=configparser.ConfigParser(interpolation=None)
    config.readfp(fp)
    fp.close()
else:
    eprint('pypad_udp.py: you must specify a configuration file')
    sys.exit(1)

#
# Create Send Socket
#
send_sock=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

rcvr=PyPAD.Receiver()
rcvr.setCallback(ProcessPad)
rcvr.start("localhost",PyPAD.PAD_TCP_PORT)
