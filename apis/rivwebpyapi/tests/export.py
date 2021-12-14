#!%PYTHON_BANGPATH%

# export.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the Export Web API call
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
audio_format=-1
channels=2
sample_rate=48000
bit_rate=0
quality=0
start_point=-1
end_point=-1
normalization_level=0
enable_metadata=False

#
# Get login parameters
#
usage='export --url=<rd-url> --username=<rd-username> --filename=<str> --audio-format=<num> --channels=1|2 --sample-rate=<sps> --bit-rate=<bps> --quality=<qual> --start-point=<msec> --end-point=<msec> --normalization-level=<dbfs> [--enable-metadata] --cart-number=<num> --cut-number=<num> [--password=<passwd>]'
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
        if(f0[0]=='--cart-number'):
            cart_number=int(f0[1])
        if(f0[0]=='--cut-number'):
            cut_number=int(f0[1])
        if(f0[0]=='--audio-format'):
            audio_format=int(f0[1])
        if(f0[0]=='--channels'):
            channels=int(f0[1])
        if(f0[0]=='--sample-rate'):
            sample_rate=int(f0[1])
        if(f0[0]=='--bit-rate'):
            bit_rate=int(f0[1])
        if(f0[0]=='--quality'):
            quality=int(f0[1])
        if(f0[0]=='--start-point'):
            start_point=int(f0[1])
        if(f0[0]=='--end-point'):
            end_point=int(f0[1])
        if(f0[0]=='--normalization-level'):
            normalization_level=int(f0[1])
    if(len(f0)==1):
        if(f0[0]=='--enable-metadata'):
            enable_metadata=True

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    eprint(usage)
    sys.exit(1)
if(not filename):
    eprint('you must supply "--filename"')
    sys.exit(1)
if(cart_number==0):
    eprint('you must supply "--cart-number"')
    sys.exit(1)
if(cut_number==0):
    eprint('you must supply "--cut-number"')
    sys.exit(1)
if(audio_format<0):
    eprint('you must supply "--audio-format"')
    sys.exit(1)
if(start_point<0):
    eprint('you must supply "--start_point"')
    sys.exit(1)
if(end_point<0):
    eprint('you must supply "--end_point"')
    sys.exit(1)

#
# Get the code list
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    webapi.Export(filename=filename,cart_number=cart_number,
                  cut_number=cut_number,
                  audio_format=audio_format,channels=channels,
                  sample_rate=sample_rate,bit_rate=bit_rate,quality=quality,
                  start_point=start_point,end_point=end_point,
                  normalization_level=normalization_level,
                  enable_metadata=enable_metadata)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)
