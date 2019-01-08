#!%PYTHON_BANGPATH%

# filepath_test.py
#
# pypad regression test script for Rivendell
#
#  Exercise every filepath wildcard in 'pypad.Update::resolveFilepath()'
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
    print()
    print('DateTime: '+update.dateTime().isoformat(' '))
    print()
    print('Abbreviated weekday name [%a | %$a | %^a]: '+update.resolveFilepath('%a | %$a | %^a',update.dateTime()))
    print('Full weekday name [%A | %$A | %^A]: '+update.resolveFilepath('%A | %$A | %^A',update.dateTime()))
    print('Abbreviated month name [%b | %$b | %^b]: '+update.resolveFilepath('%b | %$b | %^b',update.dateTime()))
    print('Full month name [%B | %$B | %^B]: '+update.resolveFilepath('%B | %$B | %^B',update.dateTime()))
    print('Century [%C | %$C | %^C]: '+update.resolveFilepath('%C | %$C | %^C',update.dateTime()))
    print('Day of the month, zero padded [01 - 31] [%d | %$d | %^d]: '+update.resolveFilepath('%d | %$d | %^d',update.dateTime()))
    print('Date (mm-dd-yy) [%D | %$D | %^D]: '+update.resolveFilepath('%D | %$D | %^D',update.dateTime()))
    print('Day of the month, space padded [ 1 - 31] [%e | %$e | %^e]: '+update.resolveFilepath('%e | %$e | %^e',update.dateTime()))
    print('Day of the month, unpadded [ 1 - 31] [%E | %$E | %^E]: '+update.resolveFilepath('%E | %$E | %^E',update.dateTime()))
    print('Date (yyyy-mm-dd) [%F | %$F | %^F]: '+update.resolveFilepath('%F | %$F | %^F',update.dateTime()))
    print('Two digit year [%g | %$g | %^g]: '+update.resolveFilepath('%g | %$g | %^g',update.dateTime()))
    print('Four digit year [%G | %$G | %^G]: '+update.resolveFilepath('%G | %$G | %^G',update.dateTime()))
    print('Abbreviated month name [%h | %$h | %^h]: '+update.resolveFilepath('%h | %$h | %^h',update.dateTime()))
    print('Hour, 24 hour, zero padded [00 - 23] [%H | %$H | %^H]: '+update.resolveFilepath('%H | %$H | %^H',update.dateTime()))
    print('Hour, 12 hour, space padded [00 - 23] [%i | %$i | %^i]: '+update.resolveFilepath('%i | %$i | %^i',update.dateTime()))
    print('Hour, 12 hour, zero padded [00 - 23] [%I | %$I | %^I]: '+update.resolveFilepath('%I | %$I | %^I',update.dateTime()))
    print('Day of year, zero padded [%j | %$j | %^j]: '+update.resolveFilepath('%j | %$j | %^j',update.dateTime()))
    print('Hour, 12 hour, unpadded [00 - 23] [%J | %$J | %^J]: '+update.resolveFilepath('%J | %$J | %^J',update.dateTime()))
    print('Hour, 24 hour, space padded [%k | %$k | %^k]: '+update.resolveFilepath('%k | %$k | %^k',update.dateTime()))
    print('Month, zero padded (01 - 12) [%m | %$m | %^m]: '+update.resolveFilepath('%m | %$m | %^m',update.dateTime()))
    print('Minute, zero padded (00 - 59) [%M | %$M | %^M]: '+update.resolveFilepath('%M | %$M | %^M',update.dateTime()))
    print('AM/PM string [%p | %$p | %^p]: '+update.resolveFilepath('%p | %$p | %^p',update.dateTime()))
    print('Rivendell host name [%r | %$r | %^r]: '+update.resolveFilepath('%r | %$r | %^r',update.dateTime()))
    print('Rivendell short host name [%R | %$R | %^R]: '+update.resolveFilepath('%R | %$R | %^R',update.dateTime()))
    print('Rivendell service name [%s | %$s | %^s]: '+update.resolveFilepath('%s | %$s | %^s',update.dateTime()))
    print('Seconds, zero padded (SS) [%S | %$S | %^S]: '+update.resolveFilepath('%S | %$S | %^S',update.dateTime()))
    print('Day of the week, numeric, 1=Monday, 7=Sunday [%u | %$u | %^u]: '+update.resolveFilepath('%u | %$u | %^u',update.dateTime()))
    print('Week number, as per ISO 8601 [00 - 23] [%V | %$V | %^V]: '+update.resolveFilepath('%V | %$V | %^V',update.dateTime()))
    print('Two digit year [%y | %$y | %^y]: '+update.resolveFilepath('%y | %$y | %^y',update.dateTime()))
    print('Four digit year [00 - 23] [%Y | %$Y | %^Y]: '+update.resolveFilepath('%Y | %$Y | %^Y',update.dateTime()))
    print("Literal '%' [%%]: "+update.resolveFilepath('%%',update.dateTime()))

#
# Create an instance of 'pypad.Receiver'
#
rcvr=pypad.Receiver()

#
# Tell it to use the callback
#
rcvr.setCallback(ProcessPad)

#
# Start the receiver, giving it the hostname or IP address and TCP port of
# the target Rivendell system. Once started, all further processing can only
# be done in the callback method!
#
rcvr.start('localhost',pypad.PAD_TCP_PORT)
