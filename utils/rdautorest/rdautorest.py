#!%PYTHON_BANGPATH%

# rdautorest.py
#
# Unattended restore script for Rivendell
#
#   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

import configparser
import os
from pathlib import Path
import sys
import syslog

USAGE='USAGE: rdautorest.py [--yes] <backup-mountpoint>'

yes=False

if(len(sys.argv)==2):
    mountpoint=sys.argv[1]
else:
    if(len(sys.argv)==3):
        if(sys.argv[1]=='--yes'):
            yes=True
            mountpoint=sys.argv[2]
        else:
            if(sys.argv[2]=='--yes'):
                yes=True
                mountpoint=sys.argv[1]
            else:
                print(USAGE)
                exit(1)
    else:
        print(USAGE)
        exit(1)

if(not yes):
    print('WARNING: This operation will COMPLETELY OVERWRITE the existing')
    print('         Rivendell data on this system, replacing it with the')
    print('         contents of the specified data backup. This operation')
    print('         cannot be undone!')
    print('')
    resp=input('        Are you sure you want to proceed (y/N)?')
    if((resp.upper()!='Y')and(resp.upper()!='YES')):
        exit(0)

#
# Load rd.conf(5)
#
rd_config=configparser.ConfigParser(interpolation=None)
rd_config.read_file(open('/etc/rd.conf'))

#
# Open the syslog
#
syslog.openlog('rdautorest.py',logoption=syslog.LOG_PID|syslog.LOG_PERROR,facility=int(rd_config.get('Identity','SyslogFacility',fallback=syslog.LOG_USER)))
syslog.syslog(syslog.LOG_INFO,'Starting Rivendell backup to "'+mountpoint+'"')

    
#
# Mount backup device
#
result=os.system(command='findmnt '+mountpoint)
if(os.waitstatus_to_exitcode(result)!=0):
    Path(mountpoint).mkdir(parents=True,exist_ok=True)
    result=os.system(command='mount '+mountpoint)
    if(os.waitstatus_to_exitcode(result)!=0):
        syslog.syslog(syslog.LOG_ERR,'unable to mount backup drive')
        exit(1)
    os.system(command='sleep 5')

#
# Stop Rivendell service
#
result=os.system(command='/bin/systemctl stop rivendell')
if(os.waitstatus_to_exitcode(result)!=0):
    syslog.syslog(syslog.LOG_ERR,'unable to stop Rivendell service')
    exit(1)

#
# Create new database
#
sql='drop\ database\ '+rd_config.get('mySQL','Database')+'\;'
cmd='echo '+sql+' | mysql -h '+rd_config.get('mySQL','Hostname')+' -u '+rd_config.get('mySQL','Loginname')+' -p'+rd_config.get('mySQL','Password')+' '+rd_config.get('mySQL','Database')
os.system(command=cmd)

sql='create\ database\ '+rd_config.get('mySQL','Database')+'\;'
cmd='echo '+sql+' | mysql -h '+rd_config.get('mySQL','Hostname')+' -u '+rd_config.get('mySQL','Loginname')+' -p'+rd_config.get('mySQL','Password')
os.system(command=cmd)

cmd='gzip -cd '+mountpoint+'/db.sql.gz | mysql -h '+rd_config.get('mySQL','Hostname')+' -u '+rd_config.get('mySQL','Loginname')+' -p'+rd_config.get('mySQL','Password')+' '+rd_config.get('mySQL','Database')
os.system(command=cmd)

#
# Copy Audio Store
#
cmd='rsync -av --delete '+mountpoint+'/snd/ /var/snd '
os.system(command=cmd)

#
# Start Rivendell service
#
result=os.system(command='/bin/systemctl restart rivendell')
if(os.waitstatus_to_exitcode(result)!=0):
    syslog.syslog(syslog.LOG_ERR,'unable to start Rivendell service')
    exit(1)

#
# Unmount backup device
#
os.system(command='umount '+mountpoint)
os.rmdir(mountpoint)

syslog.syslog(syslog.LOG_INFO,'Completed Rivendell restore from "'+mountpoint+'"')
