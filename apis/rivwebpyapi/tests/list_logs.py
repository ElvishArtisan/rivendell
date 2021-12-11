#!%PYTHON_BANGPATH%

# list_logs.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the ListLogs Web API call
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
usage='list_logs --url=<rd-url> --username=<rd-username> [--password=<passwd>] [--recent] [--trackable] [--service-name=<name>] [--log-name=<name>] [--filter-string=<str>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==1):
        if(f0[0]=='--recent'):
            recent=True
        if(f0[0]=='--trackable'):
            trackable=True
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]
        if(f0[0]=='--service-name'):
            service_name=f0[1]
        if(f0[0]=='--log-name'):
            log_name=f0[1]
        if(f0[0]=='--filter-string'):
            filter_string=f0[1]
if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)

#
# Get the log list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    logs=webapi.ListLogs(service_name=service_name,log_name=log_name,
                         trackable=trackable,filter_string=filter_string,
                         recent=recent)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)

#
# Display the list of logs
#
for log in logs:
    for key in log.values():
        print(key+': '+str(log.values()[key]))
    print('')
