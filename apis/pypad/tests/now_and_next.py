#!%PYTHON_BANGPATH%

# now_and_next.py
#
# Barebones example pypad script for Rivendell
#
#   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#
# To see the full documentation of these classes, enter the following at
# a python interactive prompt:
#
#   from rivendellaudio import pypad
#   help(pypad)
#
try:
    from rivendellaudio import pypad
except ModuleNotFoundError:
    import pypad  # Rivendell v3.x style

#
# First, we create a callback method, that will be called every time a
# log machine updates its PAD. An instance of 'pypad.Update' that contains
# the PAD information is supplied as the single argument.
#
def ProcessPad(update):
    print()
    if update.hasPadType(pypad.TYPE_NOW):
        msg='%03d:' % update.machine()
        msg+='%04d ' % update.padField(pypad.TYPE_NOW,pypad.FIELD_LINE_NUMBER)
        msg+='NOW: '+update.resolvePadFields('%a - %t',pypad.ESCAPE_NONE)
        print(msg)
    else:
        print("%03d:xxxx NOW: [none]" % update.machine())
    if update.hasPadType(pypad.TYPE_NEXT):
        msg='%03d:' % update.machine()
        msg+='%04d ' % update.padField(pypad.TYPE_NEXT,pypad.FIELD_LINE_NUMBER)
        msg+='NEXT: '+update.resolvePadFields('%A - %T',pypad.ESCAPE_NONE)
        print(msg)
    else:
        print("%03d:xxxx NEXT: [none]" % update.machine())

#
# Create an instance of 'pypad.Receiver'
#
rcvr=pypad.Receiver()

#
# Tell it to use the callback
#
rcvr.setPadCallback(ProcessPad)

#
# Start the receiver, giving it the hostname or IP address and TCP port of
# the target Rivendell system. Once started, all further processing can only
# be done in the callback method!
#
rcvr.start('localhost',pypad.PAD_TCP_PORT)
