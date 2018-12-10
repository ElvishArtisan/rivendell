#!/usr/bin/python

# pypad_test.py
#
# PyPAD regression test script for Rivendell
#
#  Exercise every method of 'PyPAD.Update' for each update.
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

import PyPAD

def ProcessPad(update):
    print
    print '*** Log %03d Update ***********************************************' % update.machine()
    print '** HEADER INFO **'
    print '          dateTime(): '+update.dateTime().isoformat(' ')
    print '           machine(): %d' % update.machine()
    print '              mode(): '+update.mode()
    print '         onairFlag(): '+str(update.onairFlag())
    print
    if update.hasLog():
        print '** LOG INFO **'
        print '           logName(): '+update.logName()
        print
    else:
        print '**NO LOG INFO PRESENT**'
        print

    if update.hasService():
        print '** SERVICE INFO **'
        print '       serviceName(): '+update.serviceName()
        print 'serviceDescription(): '+update.serviceDescription()
        print 'serviceProgramCode(): '+update.serviceProgramCode()
        print
    else:
        print '** NO SERVICE INFO PRESENT **'
        print

    if update.hasPadType(PyPAD.TYPE_NOW):
        print '** NOW PLAYING INFO **'
        try:
            print '     startDateTime(): '+update.startDateTime(PyPAD.TYPE_NOW).isoformat(' ')
        except AttributeError:
            print '     startDateTime(): None'
        print '          cartType(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_CART_TYPE)
        print '        cartNumber(): %u / ' % update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_CART_NUMBER)+update.resolvePadFields("%n",PyPAD.ESCAPE_NONE)
        print '         cutNumber(): %u / ' % update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_CUT_NUMBER)+update.resolvePadFields("%j",PyPAD.ESCAPE_NONE)
        print '            length(): %u / ' % update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_LENGTH)+update.resolvePadFields("%h",PyPAD.ESCAPE_NONE)
        try:
            print '              year(): %u / ' % update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_YEAR)+update.resolvePadFields("%y",PyPAD.ESCAPE_NONE)
        except TypeError:
            print '              year(): None / '+update.resolvePadFields("%Y",PyPAD.ESCAPE_NONE)
        print '         groupName(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_GROUP_NAME)+' / '+update.resolvePadFields('%g',PyPAD.ESCAPE_NONE)
        print '             title(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_TITLE)+' / '+update.resolvePadFields('%t',PyPAD.ESCAPE_NONE)
        print '            artist(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_ARTIST)+' / '+update.resolvePadFields('%a',PyPAD.ESCAPE_NONE)
        print '         publisher(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_PUBLISHER)+' / '+update.resolvePadFields('%p',PyPAD.ESCAPE_NONE)
        print '          composer(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_COMPOSER)+' / '+update.resolvePadFields('%m',PyPAD.ESCAPE_NONE)
        print '             album(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_ALBUM)+' / '+update.resolvePadFields('%l',PyPAD.ESCAPE_NONE)
        print '             label(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_LABEL)+' / '+update.resolvePadFields('%b',PyPAD.ESCAPE_NONE)
        print '            client(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_CLIENT)+' / '+update.resolvePadFields('%c',PyPAD.ESCAPE_NONE)
        print '            agency(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_AGENCY)+' / '+update.resolvePadFields('%e',PyPAD.ESCAPE_NONE)
        print '         conductor(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_CONDUCTOR)+' / '+update.resolvePadFields('%r',PyPAD.ESCAPE_NONE)
        print '       userDefined(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_USER_DEFINED)+' / '+update.resolvePadFields('%u',PyPAD.ESCAPE_NONE)
        print '            songId(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_SONG_ID)+' / '+update.resolvePadFields('%s',PyPAD.ESCAPE_NONE)
        print '            outcue(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_OUTCUE)+' / '+update.resolvePadFields('%o',PyPAD.ESCAPE_NONE)
        print '       description(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_DESCRIPTION)+' / '+update.resolvePadFields('%i',PyPAD.ESCAPE_NONE)
        print '   externalEventId(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_EXTERNAL_EVENT_ID)
        print '      externalData(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_EXTERNAL_DATA)
        print '  externalAnncType(): '+update.padField(PyPAD.TYPE_NOW,PyPAD.FIELD_EXTERNAL_EVENT_ID)
        print

    else:
        print '** NO NOW PLAYING INFO **'
        print

    if update.hasPadType(PyPAD.TYPE_NEXT):
        print '** NEXT PLAYING INFO **'
        try:
            print '     startDateTime(): '+update.startDateTime(PyPAD.TYPE_NEXT).isoformat(' ')
        except AttributeError:
            print '     startDateTime(): None'
        print '          cartType(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_CART_TYPE)
        print '        cartNumber(): %u / ' % update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_CART_NUMBER)+update.resolvePadFields("%N",PyPAD.ESCAPE_NONE)
        print '         cutNumber(): %u / ' % update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_CUT_NUMBER)+update.resolvePadFields("%J",PyPAD.ESCAPE_NONE)
        print '            length(): %u / ' % update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_LENGTH)+update.resolvePadFields("%H",PyPAD.ESCAPE_NONE)
        try:
            print '              year(): %u / ' % update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_YEAR)+update.resolvePadFields("%Y",PyPAD.ESCAPE_NONE)
        except TypeError:
            print '              year(): None / '+update.resolvePadFields("%Y",PyPAD.ESCAPE_NONE)
        print '         groupName(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_GROUP_NAME)+' / '+update.resolvePadFields('%G',PyPAD.ESCAPE_NONE)
        print '             title(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_TITLE)+' / '+update.resolvePadFields('%T',PyPAD.ESCAPE_NONE)
        print '            artist(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_ARTIST)+' / '+update.resolvePadFields('%A',PyPAD.ESCAPE_NONE)
        print '         publisher(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_PUBLISHER)+' / '+update.resolvePadFields('%P',PyPAD.ESCAPE_NONE)
        print '          composer(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_COMPOSER)+' / '+update.resolvePadFields('%M',PyPAD.ESCAPE_NONE)
        print '             album(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_ALBUM)+' / '+update.resolvePadFields('%L',PyPAD.ESCAPE_NONE)
        print '             label(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_LABEL)+' / '+update.resolvePadFields('%B',PyPAD.ESCAPE_NONE)
        print '            client(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_CLIENT)+' / '+update.resolvePadFields('%C',PyPAD.ESCAPE_NONE)
        print '            agency(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_AGENCY)+' / '+update.resolvePadFields('%E',PyPAD.ESCAPE_NONE)
        print '         conductor(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_CONDUCTOR)+' / '+update.resolvePadFields('%R',PyPAD.ESCAPE_NONE)
        print '       userDefined(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_USER_DEFINED)+' / '+update.resolvePadFields('%U',PyPAD.ESCAPE_NONE)
        print '            songId(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_SONG_ID)+' / '+update.resolvePadFields('%S',PyPAD.ESCAPE_NONE)
        print '            outcue(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_OUTCUE)+' / '+update.resolvePadFields('%O',PyPAD.ESCAPE_NONE)
        print '       description(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_DESCRIPTION)+' / '+update.resolvePadFields('%I',PyPAD.ESCAPE_NONE)
        print '   externalEventId(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_EXTERNAL_EVENT_ID)
        print '      externalData(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_EXTERNAL_DATA)
        print '  externalAnncType(): '+update.padField(PyPAD.TYPE_NEXT,PyPAD.FIELD_EXTERNAL_EVENT_ID)
        print

    else:
        print '** NO NEXT PLAYING INFO **'
        print

    print '******************************************************************'

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
rcvr.start("localhost",PyPAD.PAD_TCP_PORT)
