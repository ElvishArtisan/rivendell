#!%PYTHON_BANGPATH%

# edit_cart.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the EditCart Web API call
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
values={}

#
# Get login parameters
#
usage='edit_cart --url=<rd-url> --username=<rd-username> --cart-number=<num> [--set-value=<field>=<str> ...] [--password=<passwd>]'
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

#
# Get the cart list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
cart=rivwebpyapi.Cart()
cart.setValues(values)

try:
    cart=webapi.EditCart(cart_number=cart_number,values=cart.values())
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the modified cart
#
print('MODIFIED')
for key in cart.values():
    print(key+': '+str(cart.values()[key]))
print('')
