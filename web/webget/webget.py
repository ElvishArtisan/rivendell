#!/usr/bin/python

from __future__ import print_function

import os
import sys
import subprocess
import tempfile
import urllib
import ConfigParser
import mysql.connector

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def GetDbCredentials():
    config=ConfigParser.ConfigParser()
    config.readfp(open('/etc/rd.conf'))
    return (config.get('mySQL','Loginname'),config.get('mySQL','Password'),
            config.get('mySQL','Hostname'),config.get('mySQL','Database'))

def OpenDb():
    creds=GetDbCredentials()
    return mysql.connector.connect(user=creds[0],password=creds[1],
                                   host=creds[2],database=creds[3],

                                   buffered=True)

def CgiError(msg,resp_code):
   print('Content-type: text/html')
   print('Status: '+str(resp_code))
   print()
   print(msg)
   sys.exit(0)

def AuthenticateUser(name,passwd,db):
    sql='select WEBGET_LOGIN_PRIV from USERS where ';
    sql+='LOGIN_NAME="'+name+'" && PASSWORD="'+passwd+'"'
    q=db.cursor()
    q.execute(sql)
    if(len(q.fetchall())==0):
        CgiError("Invalid User name or Password!",400)

#
# Main Function
#
title=''
samprate=48000
format='MP3'
bitrate=32000
quality=2
login_name=''
password=''

#
# Get call fields
#
f0=os.environ['QUERY_STRING'].split('&')
for field in f0:
    f1=field.split('=')
    if(f1[0]=='title'):
        title=urllib.unquote(f1[1])
    if(f1[0]=='samprate'):
        samprate=int(urllib.unquote(f1[1]))
    if(f1[0]=='format'):
        format=urllib.unquote(f1[1])
    if(f1[0]=='bitrate'):
        bitrate=int(urllib.unquote(f1[1]))
    if(f1[0]=='quality'):
        quality=int(urllib.unquote(f1[1]))
    if(f1[0]=='login_name'):
        login_name=urllib.unquote(f1[1])
    if(f1[0]=='password'):
        password=urllib.unquote(f1[1])

login_name=login_name.replace('+',' ')
password=password.replace('+',' ')

db=OpenDb()
AuthenticateUser(login_name,password,db)

if(len(title)==0):
    CgiError('No title specified!',400)
title=title.replace('+',' ')
    
#
# Generate file export
#
tempdir=tempfile.mkdtemp()
try:
    filename=subprocess.check_output(('rdexport','--title='+title,
                                      '--metadata-pattern=%t',
                                      '--samplerate='+str(samprate),
                                      '--format='+format,
                                      '--bitrate='+str(bitrate),
                                      '--quality='+str(quality),
                                      tempdir))
except subprocess.CalledProcessError:
    os.rmdir(tempdir)
    print('Content-type: text/html')
    print('Status: 500')
    print()
    print('500 - unable to execute rdexport(1)')
    sys.exit(0)
if(len(filename)==0):
    os.rmdir(tempdir)
    print('Content-type: text/html')
    print('Status: 404')
    print()
    print('404 - no cart with that title');
    sys.exit(0)
filename=filename[0:len(filename)-1]
filepath=tempdir+'/'+filename
try:
    mimetype=subprocess.check_output(('file','--brief','--mime-type',filepath))
except subprocess.CalledProcessError:
    os.remove(filepath)
    os.rmdir(tempdir)
    print('Content-type: text/html')
    print('Status: 500')
    print()
    print('500 - unable to determine output mime-type')
    sys.exit(0)

#
# Render the output
#
try:
    f=open(filepath,'r')
except IOError, reason:
    print('Content-type: text/html')
    print()
    print('500 - unable to open exported file "'+filepath+'" ['+str(reason)+']')
    sys.exit(0)
    
print('Content-Disposition: attachment; filename=\"'+filename+'\"')
print('Content-type: '+mimetype)

bytes=f.read(1024)
while(len(bytes)>0):
    sys.stdout.write(bytes)
    bytes=f.read(1024)
f.close()
os.remove(filepath)
os.rmdir(tempdir)
