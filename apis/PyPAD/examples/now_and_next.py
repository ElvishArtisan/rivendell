#!%PYTHON_BANGPATH%

# now_and_next.py
#
# Example PyPAD script for Rivendell
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

#
# To see the full documentation of these classes, enter the following at
# a python interactive prompt:
#
#   import PyPAD
#   help(PyPAD)
#
import PyPAD

#
# First, we create a callback method, that will be called every time a
# log machine updates its PAD. An instance of 'PyPAD.Update' that contains
# the PAD information is supplied as the single argument.
#
def ProcessPad(update):
    print
    if update.hasPadType(PyPAD.TYPE_NOW):
        print("Log %03d NOW: " % update.machine()+update.resolvePadFields("%a - %t",PyPAD.ESCAPE_NONE))
    else:
        print("Log %03d NOW: [none]" % update.machine())
    if update.hasPadType(PyPAD.TYPE_NEXT):
        print("Log %03d NEXT: " % update.machine()+update.resolvePadFields("%A - %T",PyPAD.ESCAPE_NONE))
    else:
        print("Log %03d NEXT: [none]" % update.machine())

#
# Create an instance of 'PyPADReceiver'
#
rcvr=PyPAD.Receiver()

#
# Tell it to use the callback
#
rcvr.setCallback(ProcessPad)

#
# Start the receiver, giving it the hostname or IP address and TCP port of
# the target Rivendell system. Once started, all further processing can only
# be done in the callback method!
#
rcvr.start('localhost',PyPAD.PAD_TCP_PORT)
