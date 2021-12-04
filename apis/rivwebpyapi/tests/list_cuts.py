#!%PYTHON_BANGPATH%

# list_cuts.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListCuts Web API call
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
usage='list_cuts --url=<rd-url> --username=<rd-username> --cart-number=<num> [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]
        if(f0[0]=='--cart-number'):
            cart_number=int(f0[1])

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)

#
# Get the cut list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
cuts=webapi.ListCuts(cart_number=cart_number)

#
# Display the cut list
#
for cut in cuts:
    print('cutName: '+str(cut['cutName']))
    print('cartNumber: '+str(cut['cartNumber']))
    print('cutNumber: '+str(cut['cutNumber']))
    print('evergreen: '+str(cut['evergreen']))
    print('description: '+str(cut['description']))
    print('outcue: '+str(cut['outcue']))
    print('isrc: '+str(cut['isrc']))
    print('isci: '+str(cut['isci']))
    print('recordingMbId: '+str(cut['recordingMbId']))
    print('releaseMbId: '+str(cut['releaseMbId']))
    print('length: '+str(cut['length']))
    print('originDatetime: '+str(cut['originDatetime']))
    print('startDatetime: '+str(cut['startDatetime']))
    print('endDatetime: '+str(cut['endDatetime']))
    print('sun: '+str(cut['sun']))
    print('mon: '+str(cut['mon']))
    print('tue: '+str(cut['tue']))
    print('wed: '+str(cut['wed']))
    print('thu: '+str(cut['thu']))
    print('fri: '+str(cut['fri']))
    print('sat: '+str(cut['sat']))
    print('startDaypart: '+str(cut['startDaypart']))
    print('endDaypart: '+str(cut['endDaypart']))
    print('originName: '+str(cut['originName']))
    print('originLoginName: '+str(cut['originLoginName']))
    print('sourceHostname: '+str(cut['sourceHostname']))
    print('weight: '+str(cut['weight']))
    print('lastPlayDatetime: '+str(cut['lastPlayDatetime']))
    print('playCounter: '+str(cut['playCounter']))
    print('codingFormat: '+str(cut['codingFormat']))
    print('sampleRate: '+str(cut['sampleRate']))
    print('bitRate: '+str(cut['bitRate']))
    print('channels: '+str(cut['channels']))
    print('playGain: '+str(cut['playGain']))
    print('startPoint: '+str(cut['startPoint']))
    print('endPoint: '+str(cut['endPoint']))
    print('fadeupPoint: '+str(cut['fadeupPoint']))
    print('fadedownPoint: '+str(cut['fadedownPoint']))
    print('segueStartPoint: '+str(cut['segueStartPoint']))
    print('segueEndPoint: '+str(cut['segueEndPoint']))
    print('segueGain: '+str(cut['segueGain']))
    print('hookStartPoint: '+str(cut['hookStartPoint']))
    print('hookEndPoint: '+str(cut['hookEndPoint']))
    print('talkStartPoint: '+str(cut['talkStartPoint']))
    print('talkEndPoint: '+str(cut['talkEndPoint']))
    print('')
