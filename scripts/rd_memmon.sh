#!/bin/sh

# rd_memmon.sh
#
#   Display a top session showing Rivendell components
#
#   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rd_memmon.sh,v 1.2 2010/10/06 20:40:47 cvs Exp $
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

RIPCD_PID=`cat /var/run/rivendell/ripcd.pid`
CAED_PID=`cat /var/run/rivendell/caed.pid`
RDCATCHD_PID=`cat /var/run/rivendell/rdcatchd.pid`

if [ -z $1 ] ; then
  top -p $RIPCD_PID,$CAED_PID,$RDCATCHD_PID
else
  MODULE_PID=`ps -C $1 -o pid=`
  top -p $RIPCD_PID,$CAED_PID,$RDCATCHD_PID,$MODULE_PID
fi


# End of rd_memmon.sh
