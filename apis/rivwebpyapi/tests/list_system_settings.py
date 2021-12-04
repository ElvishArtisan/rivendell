#!%PYTHON_BANGPATH%

# list_system_settings.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListSystemSettings Web API call
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
cart_number=0
include_cuts=False

#
# Get login parameters
#
usage='list_system_settings --url=<rd-url> --username=<rd-username> [--password=<passwd>]'
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
settings=webapi.ListSystemSettings()

#
# Display the settings list
#
for setting in settings:
    print('realmName: '+str(setting['realmName']))
    print('sampleRate: '+str(setting['sampleRate']))
    print('duplicateTitles: '+str(setting['duplicateTitles']))
    print('fixDuplicateTitles: '+str(setting['fixDuplicateTitles']))
    print('maxPostLength: '+str(setting['maxPostLength']))
    print('isciXreferencePath: '+str(setting['isciXreferencePath']))
    print('tempCartGroup: '+str(setting['tempCartGroup']))
    print('longDateFormat: '+str(setting['longDateFormat']))
    print('shortDateFormat: '+str(setting['shortDateFormat']))
    print('showTwelveHourTime: '+str(setting['showTwelveHourTime']))
    print('')
