#!%PYTHON_BANGPATH%

# audio_info.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the AudioInfo Web API call
#
#   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import getpass
import sys

from rivwebpyapi import rivwebpyapi

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

url='';
username=''
password=''
cart_number=0
cut_number=0

#
# Get login parameters
#
usage='audio_info --url=<rd-url> --username=<rd-username> --cart-number=<num> --cut-number=<num> [--password=<passwd>]'
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
        if(f0[0]=='--cut-number'):
            cut_number=int(f0[1])

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)
if(cart_number==0):
    eprint('you must supply "--cart-number"')
    sys.exit(1)
if(cut_number==0):
    eprint('you must supply "--cut-number"')
    sys.exit(1)

#
# Get the code list
#
site=rivwebpyapi.Site(url=url,username=username,password=password)
try:
    info=site.AudioInfo(cart_number=cart_number,cut_number=cut_number)
except rivwebpyapi.RivWebPyError as err:
    print('*** ERROR ***')
    print('Response Code: '+str(err.responseCode))
    print('ErrorString: '+str(err.errorString))
    print('*************')
    print('')
    sys.exit(1)

#
# Display the settings list
#
print('cartNumber: '+str(info['cartNumber']))
print('cutNumber: '+str(info['cutNumber']))
print('format: '+str(info['format']))
print('channels: '+str(info['channels']))
print('sampleRate: '+str(info['sampleRate']))
print('bitRate: '+str(info['bitRate']))
print('frames: '+str(info['frames']))
print('length: '+str(info['length']))
