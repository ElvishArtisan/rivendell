#!%PYTHON_BANGPATH%

# import.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the Import Web API call
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
import rivwebpyapi
import sys
def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

url='';
username=''
password=''
filename=''
cart_number=0
cut_number=0
channels=2
normalization_level=0
autotrim_level=0
use_metadata=False
group_name=''
title=''

#
# Get login parameters
#
usage='import --url=<rd-url> --username=<rd-username> --filename=<str> --channels=1|2 --normalization-level=<dbfs> --autotrim-level=<dbfs> --use-metadata [--cart-number=<num>] [--cut-number=<num>] [--group-name=<str>] [--title=<str>] [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]
        if(f0[0]=='--filename'):
            filename=f0[1]
        if(f0[0]=='--group-name'):
            group_name=f0[1]
        if(f0[0]=='--title'):
            title=f0[1]
        if(f0[0]=='--cart-number'):
            cart_number=int(f0[1])
        if(f0[0]=='--cut-number'):
            cut_number=int(f0[1])
        if(f0[0]=='--channels'):
            channels=int(f0[1])
        if(f0[0]=='--normalization-level'):
            normalization_level=int(f0[1])
        if(f0[0]=='--autotrim-level'):
            autotrim_level=int(f0[1])
    if(len(f0)==1):
        if(f0[0]=='--use-metadata'):
            use_metadata=True

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)
if((not group_name)and(cart_number==0)):
    eprint('you must supply "--cart-number"')
    sys.exit(1)
if((not group_name)and(cut_number==0)):
    eprint('you must supply "--cut-number"')
    sys.exit(1)
if(not filename):
    eprint('you must supply "--filename"')
    sys.exit(1)

#
# Get the code list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    result=webapi.Import(filename=filename,cart_number=cart_number,
                         cut_number=cut_number,channels=channels,
                         normalization_level=normalization_level,
                         autotrim_level=autotrim_level,
                         use_metadata=use_metadata,group_name=group_name,
                         title=title)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the destination info
#
print('')
if(group_name):
    print('ADDED:')
    print('cartNumber: '+str(result['CartNumber']))
    print('cutNumber: '+str(result['CutNumber']))
    print('')

