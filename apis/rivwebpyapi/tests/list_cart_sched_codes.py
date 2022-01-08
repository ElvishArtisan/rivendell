#!%PYTHON_BANGPATH%

# list_cart_sched_codes.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListCartSchedCodes Web API call
#
#   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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

from rivendellaudio import rivwebpyapi

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
usage='list_cart_sched_codes --url=<rd-url> --username=<rd-username> --cart-number=<num> [--password=<passwd>]'
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
# Get the code list
#
site=rivwebpyapi.Site(url=url,username=username,password=password)
try:
    codes=site.ListCartSchedCodes(cart_number=cart_number)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the code list
#
for code in codes:
    for key in code.values():
        print(key+': '+str(code.values()[key]))
    print('')
