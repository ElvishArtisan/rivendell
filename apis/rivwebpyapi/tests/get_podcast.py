#!%PYTHON_BANGPATH%

# get_podcast.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the GetPodcast Web API call
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
filename=''
cast_id=-1

#
# Get login parameters
#
usage='get_podcast --url=<rd-url> --username=<rd-username> --filename=<str> --cast-id=<id>'
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
        if(f0[0]=='--cast-id'):
            cast_id=int(f0[1])

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    print(usage)
    sys.exit(1)
if(cast_id<0):
    eprint('you must supply "--cast-id"')
    sys.exit(1)
if(not filename):
    eprint('you must supply "--filename"')
    sys.exit(1)

#
# Execute
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    webapi.GetPodcast(cast_id=cast_id,filename=filename)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)
