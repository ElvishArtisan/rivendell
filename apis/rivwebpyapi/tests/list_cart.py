#!%PYTHON_BANGPATH%

# list_cart.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListCart Web API call
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
cart_number=0
include_cuts=False

#
# Get login parameters
#
usage='list_cart --url=<rd-url> --username=<rd-username> --cart-number=<num> [--password=<passwd>] [--include-cuts]'
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
if(cart_number==0):
    eprint('you must supply "--cart-number"')
    sys.exit(1)

#
# Get the cart list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    cart=webapi.ListCart(cart_number=cart_number,include_cuts=include_cuts)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the cart list
#
print('number: '+str(cart['number']))
print('type: '+str(cart['type']))
print('groupName: '+str(cart['groupName']))
print('title: '+str(cart['title']))
print('artist: '+str(cart['artist']))
print('album: '+str(cart['album']))
print('year: '+str(cart['year']))
print('label: '+str(cart['label']))
print('client: '+str(cart['client']))
print('agency: '+str(cart['agency']))
print('publisher: '+str(cart['publisher']))
print('composer: '+str(cart['composer']))
print('conductor: '+str(cart['conductor']))
print('userDefined: '+str(cart['userDefined']))
print('usageCode: '+str(cart['usageCode']))
print('forcedLength: '+str(cart['forcedLength']))
print('averageLength: '+str(cart['averageLength']))
print('lengthDeviation: '+str(cart['lengthDeviation']))
print('averageSegueLength: '+str(cart['averageSegueLength']))
print('averageHookLength: '+str(cart['averageHookLength']))
print('minimumTalkLength: '+str(cart['minimumTalkLength']))
print('maximumTalkLength: '+str(cart['maximumTalkLength']))
print('cutQuantity: '+str(cart['cutQuantity']))
print('lastCutPlayed: '+str(cart['lastCutPlayed']))
print('enforceLength: '+str(cart['enforceLength']))
print('asyncronous: '+str(cart['asyncronous']))
print('owner: '+str(cart['owner']))
print('metadataDatetime: '+str(cart['metadataDatetime']))
print('songId: '+str(cart['songId']))
print('')
