#!/bin/sh

# start_traverso.sh <wav-path>
#
# Load an audio file into the traverso audio DAW using OSC.
# If traverso is not already running, it is first started.
#
# (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: start_traverso.sh,v 1.3 2010/07/29 19:32:38 cvs Exp $
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

if [ -z $1 ] ; then
  echo "start_traverso.sh <wav-path>"
  exit 256
fi

PS_RESULT=`ps --no-header -C traverso -o fname`
if [ -z $PS_RESULT ] ; then
  traverso > /dev/null 2> /dev/null &
  sleep 2
fi

oscsend --path=/traverso/transport/insert_track --value=S:$1


# End of start_traverso.sh
