#!/usr/bin/python

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
#   import rivendell.PyPAD
#   help(rivendell.PyPAD)
#
import rivendell.PyPAD

#
# First, we create a callback method, that will be called every time a
# log machine updates its PAD. Two arguments are supplied:
#
#   update - An instance of 'PyPADUpdate' that contains the PAD information.
#
#   priv - An arbitrary object that was passed to 'PyPADReceiver' using
#          its 'setPrivateObject()' method. This allows initialiation to
#          be done (create sockets, file handles, etc) before starting the
#          receiver and then passing those objects to the processing callback.
#       
def ProcessPad(update,priv):
    print
    if update.hasNowPad():
        print "Log %03d NOW: " % update.logMachine()+update.padFields("%a - %t")
    else:
        print "Log %03d NOW: [none]" % update.logMachine()
    if update.hasNextPad():
        print "Log %03d NEXT: " % update.logMachine()+update.padFields("%A - %T")
    else:
        print "Log %03d NEXT: [none]" % update.logMachine()

#
# Create an instance of 'PyPADReceiver'
#
rcvr=rivendell.PyPAD.PyPADReceiver()

#
# Tell it to use the callback
#
rcvr.setCallback(ProcessPad)

#
# Start the receiver, giving it the hostname or IP address of the target
# Rivendell system. Once started, all further processing can only be done
# in the callback method!
#
rcvr.start("localhost")
