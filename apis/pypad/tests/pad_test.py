#!%PYTHON_BANGPATH%

# pad_test.py
#
# pypad regression test script for Rivendell
#
#  Exercise every PAD accessor method of 'pypad.Update' for each update.
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

import pypad

def ProcessPad(update):
    print
    print('*** Log %03d Update ***********************************************' % update.machine())
    print('** HEADER INFO **')
    print('          dateTime(): '+update.dateTime().isoformat(' '))
    print('          hostName(): '+update.hostName())
    print('     shortHostName(): '+update.shortHostName())
    print('           machine(): %d' % update.machine())
    print( '              mode(): '+update.mode())
    print('         onairFlag(): '+str(update.onairFlag()))
    print
    if update.hasLog():
        print('** LOG INFO **')
        print('           logName(): '+update.logName())
        print()
    else:
        print('**NO LOG INFO PRESENT**')
        print()

    if update.hasService():
        print('** SERVICE INFO **')
        print('       serviceName(): '+update.serviceName())
        print('serviceDescription(): '+update.serviceDescription())
        print('serviceProgramCode(): '+update.serviceProgramCode())
        print()
    else:
        print('** NO SERVICE INFO PRESENT **')
        print()

    if update.hasPadType(pypad.TYPE_NOW):
        print('** NOW PLAYING INFO **')
        try:
            print('     startDateTime(): '+update.startDateTime(pypad.TYPE_NOW).isoformat(' '))
        except AttributeError:
            print('     startDateTime(): None')
        print('        lineNumber(): '+str(update.padField(pypad.TYPE_NOW,pypad.FIELD_LINE_NUMBER))+'/'+update.resolvePadFields('%z',pypad.ESCAPE_NONE))
        print('            lineId(): '+str(update.padField(pypad.TYPE_NOW,pypad.FIELD_LINE_ID))+'/'+update.resolvePadFields('%x',pypad.ESCAPE_NONE))
        print('          cartType(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_CART_TYPE))
        print('        cartNumber(): %u / ' % update.padField(pypad.TYPE_NOW,pypad.FIELD_CART_NUMBER)+update.resolvePadFields("%n",pypad.ESCAPE_NONE))
        print('         cutNumber(): %u / ' % update.padField(pypad.TYPE_NOW,pypad.FIELD_CUT_NUMBER)+update.resolvePadFields("%j",pypad.ESCAPE_NONE))
        print('            length(): %u / ' % update.padField(pypad.TYPE_NOW,pypad.FIELD_LENGTH)+update.resolvePadFields("%h",pypad.ESCAPE_NONE))
        try:
            print('              year(): %u / ' % update.padField(pypad.TYPE_NOW,pypad.FIELD_YEAR)+update.resolvePadFields("%y",pypad.ESCAPE_NONE))
        except TypeError:
            print('              year(): None / '+update.resolvePadFields("%Y",pypad.ESCAPE_NONE))
        print('         groupName(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_GROUP_NAME)+' / '+update.resolvePadFields('%g',pypad.ESCAPE_NONE))
        print('             title(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_TITLE)+' / '+update.resolvePadFields('%t',pypad.ESCAPE_NONE))
        print('            artist(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_ARTIST)+' / '+update.resolvePadFields('%a',pypad.ESCAPE_NONE))
        print('         publisher(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_PUBLISHER)+' / '+update.resolvePadFields('%p',pypad.ESCAPE_NONE))
        print('          composer(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_COMPOSER)+' / '+update.resolvePadFields('%m',pypad.ESCAPE_NONE))
        print('             album(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_ALBUM)+' / '+update.resolvePadFields('%l',pypad.ESCAPE_NONE))
        print('             label(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_LABEL)+' / '+update.resolvePadFields('%b',pypad.ESCAPE_NONE))
        print('            client(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_CLIENT)+' / '+update.resolvePadFields('%c',pypad.ESCAPE_NONE))
        print('            agency(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_AGENCY)+' / '+update.resolvePadFields('%e',pypad.ESCAPE_NONE))
        print('         conductor(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_CONDUCTOR)+' / '+update.resolvePadFields('%r',pypad.ESCAPE_NONE))
        print('       userDefined(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_USER_DEFINED)+' / '+update.resolvePadFields('%u',pypad.ESCAPE_NONE))
        print('            songId(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_SONG_ID)+' / '+update.resolvePadFields('%s',pypad.ESCAPE_NONE))
        print('            outcue(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_OUTCUE)+' / '+update.resolvePadFields('%o',pypad.ESCAPE_NONE))
        print('       description(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_DESCRIPTION)+' / '+update.resolvePadFields('%i',pypad.ESCAPE_NONE))
        print('   externalEventId(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_EXTERNAL_EVENT_ID))
        print('      externalData(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_EXTERNAL_DATA))
        print('  externalAnncType(): '+update.padField(pypad.TYPE_NOW,pypad.FIELD_EXTERNAL_EVENT_ID))
        print()

    else:
        print('** NO NOW PLAYING INFO **')
        print()

    if update.hasPadType(pypad.TYPE_NEXT):
        print('** NEXT PLAYING INFO **')
        try:
            print('     startDateTime(): '+update.startDateTime(pypad.TYPE_NEXT).isoformat(' '))
        except AttributeError:
            print('     startDateTime(): None')
        print('        lineNumber(): '+str(update.padField(pypad.TYPE_NEXT,pypad.FIELD_LINE_NUMBER))+'/'+update.resolvePadFields('%Z',pypad.ESCAPE_NONE))
        print('            lineId(): '+str(update.padField(pypad.TYPE_NEXT,pypad.FIELD_LINE_ID))+'/'+update.resolvePadFields('%X',pypad.ESCAPE_NONE))
        print('          cartType(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_CART_TYPE))
        print('        cartNumber(): %u / ' % update.padField(pypad.TYPE_NEXT,pypad.FIELD_CART_NUMBER)+update.resolvePadFields("%N",pypad.ESCAPE_NONE))
        print('         cutNumber(): %u / ' % update.padField(pypad.TYPE_NEXT,pypad.FIELD_CUT_NUMBER)+update.resolvePadFields("%J",pypad.ESCAPE_NONE))
        print('            length(): %u / ' % update.padField(pypad.TYPE_NEXT,pypad.FIELD_LENGTH)+update.resolvePadFields("%H",pypad.ESCAPE_NONE))
        try:
            print('              year(): %u / ' % update.padField(pypad.TYPE_NEXT,pypad.FIELD_YEAR)+update.resolvePadFields("%Y",pypad.ESCAPE_NONE))
        except TypeError:
            print('              year(): None / '+update.resolvePadFields("%Y",pypad.ESCAPE_NONE))
        print('         groupName(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_GROUP_NAME)+' / '+update.resolvePadFields('%G',pypad.ESCAPE_NONE))
        print('             title(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_TITLE)+' / '+update.resolvePadFields('%T',pypad.ESCAPE_NONE))
        print('            artist(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_ARTIST)+' / '+update.resolvePadFields('%A',pypad.ESCAPE_NONE))
        print('         publisher(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_PUBLISHER)+' / '+update.resolvePadFields('%P',pypad.ESCAPE_NONE))
        print('          composer(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_COMPOSER)+' / '+update.resolvePadFields('%M',pypad.ESCAPE_NONE))
        print('             album(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_ALBUM)+' / '+update.resolvePadFields('%L',pypad.ESCAPE_NONE))
        print('             label(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_LABEL)+' / '+update.resolvePadFields('%B',pypad.ESCAPE_NONE))
        print('            client(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_CLIENT)+' / '+update.resolvePadFields('%C',pypad.ESCAPE_NONE))
        print('            agency(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_AGENCY)+' / '+update.resolvePadFields('%E',pypad.ESCAPE_NONE))
        print('         conductor(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_CONDUCTOR)+' / '+update.resolvePadFields('%R',pypad.ESCAPE_NONE))
        print('       userDefined(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_USER_DEFINED)+' / '+update.resolvePadFields('%U',pypad.ESCAPE_NONE))
        print('            songId(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_SONG_ID)+' / '+update.resolvePadFields('%S',pypad.ESCAPE_NONE))
        print('            outcue(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_OUTCUE)+' / '+update.resolvePadFields('%O',pypad.ESCAPE_NONE))
        print('       description(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_DESCRIPTION)+' / '+update.resolvePadFields('%I',pypad.ESCAPE_NONE))
        print('   externalEventId(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_EXTERNAL_EVENT_ID))
        print('      externalData(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_EXTERNAL_DATA))
        print('  externalAnncType(): '+update.padField(pypad.TYPE_NEXT,pypad.FIELD_EXTERNAL_EVENT_ID))
        print()

    else:
        print('** NO NEXT PLAYING INFO **')
        print()

    print ('******************************************************************')

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
