#!%PYTHON_BANGPATH%

# edit_cut.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the EditCut Web API call
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
cart_number=0
cut_number=0
values={}

#
# Get login parameters
#
usage='edit_cut --url=<rd-url> --username=<rd-username> --cart-number=<num> --cut-number=<num> [--set-value=<field>=<str> ...] [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    key=f0[0]
    del f0[0]
    value='='.join(f0)
    if(value):
        if(key=='--url'):
            url=value
        if(key=='--username'):
            username=value
        if(key=='--password'):
            password=value
        if(key=='--cart-number'):
            cart_number=int(value)
        if(key=='--cut-number'):
            cut_number=int(value)
        if(key=='--set-value'):
            f1=value.split('=')
            key1=f1[0]
            del f1[0]
            values[key1]='='.join(f1)

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
# Execute
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
cut=rivwebpyapi.Cut()
cut.setValues(values)

try:
    cut=webapi.EditCut(cart_number=cart_number,cut_number=cut_number,
                       values=cut.values())
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the modified cut
#
print('MODIFIED')
for key in cut.values():
    print(key+': '+str(cut.values()[key]))
print('')
