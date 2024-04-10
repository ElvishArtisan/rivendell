#!%PYTHON_BANGPATH%

# pypad_sftpwrite.py
#
# Write PAD updates to arbitrary URLs
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
import syslog
import configparser
import paramiko
import socket
import urllib
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style
from io import BytesIO

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    n=1
    section='Url'+str(n)
    while(update.config().has_section(section)):
        if update.shouldBeProcessed(section):
            fmtstr=update.config().get(section,'FormatString')
            url=urllib.parse.urlparse(update.resolveFilepath(update.config().get(section,'Url'),update.dateTime()))
            username=update.config().get(section,'Username')
            password=update.config().get(section,'Password')
            ssh=paramiko.SSHClient()
            #ssh.load_host_keys('/usr/lib/rivendell/ssh/known_hosts')
            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            try:
                ssh.connect(url.netloc,username=username,password=password)
                sftp=ssh.open_sftp()
                path=url.path
                if path[0]=='/':  # Single leading '/' means relative path
                    path=path[1::]
                fh=sftp.file(filename=path,mode='w')
                fh.write(update.resolvePadFields(fmtstr,int(update.config().get(section,'Encoding'))).encode('utf-8'))
                fh.close()
                sftp.close()
            except paramiko.ssh_exception.NoValidConnectionsError as errors:
                update.syslog(syslog.LOG_WARNING,'connection failed: '+str(errors))
        n=n+1
        section='Url'+str(n)


#
# 'Main' function
#
rcvr=pypad.Receiver()
try:
    paramiko.util.log_to_file('/home/fredg/temp/paramiko.txt')
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_sftpwrite.py: USAGE: cmd <hostname> <port> <config>')
    sys.exit(1)
rcvr.setPadCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
