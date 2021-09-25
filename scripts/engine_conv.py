#!/usr/bin/python3

# engine_conv.py
#
# Convert a Rivendell database to use a different MySQL table engine
#
#   This script require the 'mysql-connector-python'
#
#   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

# Usage: engine_conv.py <engine>

from __future__ import print_function

import ConfigParser
import mysql.connector
import sys

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)


def GetDbCredentials():
    config=ConfigParser.ConfigParser()
    config.readfp(open('/etc/rd.conf'))
    return (config.get('mySQL','Loginname'),config.get('mySQL','Password'),
            config.get('mySQL','Hostname'),config.get('mySQL','Database'))


def OpenDb():
    creds=GetDbCredentials()
    print('P/W: '+creds[1])
    return mysql.connector.connect(user=creds[0],password=creds[1],
                                   host=creds[2],database=creds[3],buffered=True)

if(len(sys.argv)<2):
    eprint("engine_conv.py: missing <engine> argument")
    exit(256)

engine=sys.argv[1]

db=OpenDb()
creds=GetDbCredentials()

sql='select TABLE_NAME,ENGINE from information_schema.TABLES where TABLE_SCHEMA="'+creds[3]+'" && ENGINE!="'+engine+'"'
q=db.cursor()
q.execute(sql)
for row in q.fetchall():
    print('Converting table "'+row[0]+'" to '+engine)
    sql='alter table `'+row[0]+'` engine='+engine
    q1=db.cursor()
    q1.execute(sql)
    db.commit()
    q1.close()
q.close()

db.close()
