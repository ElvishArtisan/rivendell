#!/usr/bin/python3

# init_template_host.py
#
# Initialize a template-only host profile so it can be used as a
# template. (Requires the 'mysql-connector-python' package).
#
# (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

# Usage: init_template_host.py

from __future__ import print_function

import argparse
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
    return mysql.connector.connect(user=creds[0],password=creds[1],
                                   host=creds[2],database=creds[3],buffered=True)

#
# Read arguments
#
parser=argparse.ArgumentParser(description='Initialize a Rivendell host entry for use as a template')
parser.add_argument("--host",required=True,help="name of host entry to initialize")
parser.add_argument("--device",required=True,help='add a virtual device (CARD:INPUTS:OUTPUTS)')
args = parser.parse_args()
device = args.device.split(":")
if(len(device)!=3):
    eprint("init_template_host.py: invalid argument to --device")
    sys.exit(1)
cardnum=int(device[0])
if((cardnum<0) or (cardnum>7)):
    eprint("init_template_host.py: invalid card number")
    sys.exit(1)

inputs=int(device[1])
if((inputs<0) or (inputs>8)):
    eprint("init_template_host.py: invalid input quantity")
    sys.exit(1)

outputs=int(device[2])
if((outputs<0) or (outputs>8)):
    eprint("init_template_host.py: invalid output quantity")
    sys.exit(1)

#
# Open the database
#
db=OpenDb()

#
# Sanity checks
#
sql='select NAME from STATIONS where NAME="'+args.host+'"';
q=db.cursor()
q.execute(sql)
if(len(q.fetchall()) != 1):
    eprint('init_template.py: no such host\n')
    sys.exit(1)
q.close()

#
# Write host records
#
sql='update AUDIO_CARDS set '
sql+='DRIVER=2,'
sql+='NAME="Dummy Template Card",'
sql+='INPUTS='+str(inputs)+','
sql+='OUTPUTS='+str(outputs)
sql+=' where STATION_NAME="'+args.host+'" && CARD_NUMBER='+str(cardnum)
q=db.cursor()
q.execute(sql)
q.close()

sql='update STATIONS set '
sql+='STATION_SCANNED="Y" where '
sql+='NAME="'+args.host+'"'
q=db.cursor()
q.execute(sql)
q.close()

db.close()


