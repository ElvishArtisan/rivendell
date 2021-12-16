#!%PYTHON_BANGPATH%

# save_log.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the SaveLog Web API call
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
from_log_name=''
to_log_name=''

#
# Get login parameters
#
usage='save_log --url=<rd-url> --username=<rd-username> --from-cart-name=<str> --to-log-name=<str> [--password=<passwd>]'
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
        if(key=='--from-log-name'):
            from_log_name=value
        if(key=='--to-log-name'):
            to_log_name=value

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)
if(not from_log_name):
    eprint('you must supply "--from-log-name"')
    sys.exit(1)
if(not to_log_name):
    eprint('you must supply "--to-log-name"')
    sys.exit(1)
if(from_log_name==to_log_name):
    eprint('"--from-log-name" and "--to-log_name" must be different')
    sys.exit(1)

site=rivwebpyapi.Site(url=url,username=username,password=password)

#
# Get the "from" log
#
try:
    log_header=site.ListLogs(log_name=from_log_name)[0]
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR Calling ListLogs() ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*******************************')
    eprint('')
    sys.exit(1)

try:
    log_lines=site.ListLog(log_name=from_log_name)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR Calling ListLog() ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*******************************')
    eprint('')
    sys.exit(1)

#
# Save "to" log
#
try:
    site.SaveLog(log_name=to_log_name,header_values=log_header,line_values=log_lines)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR Calling SaveLog() ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*******************************')
    eprint('')
    sys.exit(1)
