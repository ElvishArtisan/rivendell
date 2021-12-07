#!%PYTHON_BANGPATH%

# list_log.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListLog Web API call
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
def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

url='';
username=''
password=''
service_name=''
log_name=''
trackable=False
filter_string=''
recent=False

#
# Get login parameters
#
usage='list_log --url=<rd-url> --username=<rd-username> --log-name=<name> [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]
        if(f0[0]=='--log-name'):
            log_name=f0[1]
if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    eprint(usage)
    sys.exit(1)
if(not log_name):
    eprint('you must supply "--log-name"')
    sys.exit(1)

#
# Get the log list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    loglines=webapi.ListLog(log_name=log_name)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the log list
#
for ll in loglines:
    print('line: '+str(ll['line']))
    print('id: '+str(ll['id']))
    print('type: '+str(ll['type']))
    print('cartType: '+str(ll['cartType']))
    print('cartNumber: '+str(ll['cartNumber']))
    print('cutNumber: '+str(ll['cutNumber']))
    print('groupName: '+str(ll['groupName']))
    print('groupColor: '+str(ll['groupColor']))
    print('title: '+str(ll['title']))
    print('artist: '+str(ll['artist']))
    print('publisher: '+str(ll['publisher']))
    print('composer: '+str(ll['composer']))
    print('album: '+str(ll['album']))
    print('label: '+str(ll['label']))
    print('year: '+str(ll['year']))
    print('client: '+str(ll['client'])) 
    print('agency: '+str(ll['agency'])) 
    print('conductor: '+str(ll['conductor'])) 
    print('userDefined: '+str(ll['userDefined']))
    print('usageCode: '+str(ll['usageCode']))
    print('enforceLength: '+str(ll['enforceLength']))
    print('forcedLength: '+str(ll['forcedLength']))
    print('evergreen: '+str(ll['evergreen']))
    print('source: '+str(ll['source']))
    print('timeType: '+str(ll['timeType']))
    print('startTime: '+str(ll['startTime']))
    print('transitionType: '+str(ll['transitionType']))
    print('cutQuantity: '+str(ll['cutQuantity']))
    print('lastCutPlayed: '+str(ll['lastCutPlayed']))
    print('markerComment: '+str(ll['markerComment']))
    print('markerLabel: '+str(ll['markerComment']))
    print('description: '+str(ll['description']))
    print('isrc: '+str(ll['isrc']))
    print('isci: '+str(ll['isci']))
    print('recordingMbId: '+str(ll['recordingMbId']))
    print('releaseMbId: '+str(ll['releaseMbId']))
    print('originUser: '+str(ll['originUser']))
    print('originDateTime: '+str(ll['originDateTime']))
    print('startPointCart: '+str(ll['startPointCart']))
    print('startPointLog: '+str(ll['startPointLog']))
    print('endPointCart: '+str(ll['endPointCart']))
    print('endPointLog: '+str(ll['endPointLog']))
    print('segueStartPointCart: '+str(ll['segueStartPointCart']))
    print('segueStartPointLog: '+str(ll['segueStartPointCart']))
    print('segueEndPointCart: '+str(ll['segueEndPointCart']))
    print('segueEndPointLog: '+str(ll['segueEndPointLog']))
    print('segueGain: '+str(ll['segueGain']))
    print('fadeupPointCart: '+str(ll['fadeupPointCart']))
    print('fadeupPointLog: '+str(ll['fadeupPointLog']))
    print('fadeupGain: '+str(ll['fadeupGain']))
    print('fadedownPointCart: '+str(ll['fadedownPointCart']))
    print('fadedownPointLog: '+str(ll['fadedownPointLog']))
    print('fadedownGain: '+str(ll['fadedownGain']))
    print('duckUpGain: '+str(ll['duckUpGain']))
    print('duckDownGain: '+str(ll['duckDownGain']))
    print('talkStartPoint: '+str(ll['talkStartPoint']))
    print('talkEndPoint: '+str(ll['talkEndPoint']))
    print('hookMode: '+str(ll['hookMode']))
    print('hookStartPoint: '+str(ll['hookStartPoint']))
    print('hookEndPoint: '+str(ll['hookEndPoint']))
    print('eventLength: '+str(ll['eventLength']))
    print('linkEventName: '+str(ll['linkEventName']))
    print('linkStartTime: '+str(ll['linkStartTime']))
    print('linkStartSlop: '+str(ll['linkStartSlop']))
    print('linkEndSlop: '+str(ll['linkEndSlop']))
    print('linkId: '+str(ll['linkId']))
    print('linkEmbedded: '+str(ll['linkEmbedded']))
    print('extStartTime: '+str(ll['extStartTime']))
    print('extLength: '+str(ll['extLength']))
    print('extCartName: '+str(ll['extCartName']))
    print('extData: '+str(ll['extData']))
    print('extEventId: '+str(ll['extEventId']))
    print('extAnncType: '+str(ll['extAnncType']))
    print('')


#    print(': '+ll['']) 
