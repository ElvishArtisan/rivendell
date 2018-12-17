#!%PYTHON_BANGPATH%

# pypad_filewrite.py
#
# Write PAD updates to files
#
#   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

import sys
import configparser
import PyPAD

def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

def ProcessPad(update):
    n=1
    try:
        while(True):
            section='File'+str(n)
            if update.shouldBeProcessed(section):
                fmtstr=update.config().get(section,'FormatString')
                mode='w'
                if update.config().get(section,'Append')=='1':
                    mode='a'
                f=open(update.resolveFilepath(update.config().get(section,'Filename'),update.dateTime()),mode)
                f.write(update.resolvePadFields(fmtstr,int(update.config().get(section,'Encoding'))))
                f.close()
            n=n+1

    except configparser.NoSectionError:
        return

#
# 'Main' function
#
rcvr=PyPAD.Receiver()
try:
    rcvr.setConfigFile(sys.argv[3])
except IndexError:
    eprint('pypad_filewrite.py: you must specify a configuration file')
    sys.exit(1)
rcvr.setCallback(ProcessPad)
rcvr.start(sys.argv[1],int(sys.argv[2]))
