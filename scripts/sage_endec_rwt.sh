#!/bin/sh

# sage_endec_rwt.sh
#
# Run a Required Weekly Test on a Sage Digital ENDEC
#
#   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: sage_endec_rwt.sh,v 1.1.2.1 2012/10/11 19:16:27 cvs Exp $
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

USAGE="sage_endec_rwt.sh <ip-addr> <web-user> <web-password>"

if [ -z $1 ] ; then
  echo $USAGE
  exit 256
fi
if [ -z $2 ] ; then
  echo $USAGE
  exit 256
fi
if [ -z $3 ] ; then
  echo $USAGE
  exit 256
fi

HOSTNAME=$1
USERNAME=$2
PASSWORD=$3

curl --anyauth --user $USERNAME:$PASSWORD --data rwt=1\&handle=1 http://$HOSTNAME/cgi-bin/pend.cgi

# End of sage_endec_rwt.sh
