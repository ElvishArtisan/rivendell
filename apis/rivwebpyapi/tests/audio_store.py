#!%PYTHON_BANGPATH%

# audio_store.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the AudioStore Web API call
#
#   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

import getpass
import rivwebpyapi
import sys

url='';
username=''
password=''

#
# Get login parameters
#
usage='audio_store --url=<rd-url> --username=<rd-username> [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)

#
# Get the code list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
settings=webapi.AudioStore()

#
# Display the settings list
#
for setting in settings:
    print('freeBytes: '+str(setting['freeBytes']))
    print('totalBytes: '+str(setting['totalBytes']))
    print('')
