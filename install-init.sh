#! /bin/bash

# install-init.sh
#
# Install the init scripts for Rivendell
#
# (C) Copyright 2005 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: install-init.sh,v 1.7 2010/07/29 19:32:30 cvs Exp $
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

if test ! -f ./building-debian ; then
  mkdir -p /etc/init.d
  cp rivendell /etc/init.d/rivendell
  cp rdrepld-suse /etc/init.d/rdrepld
  mkdir -p /etc/sysconfig
  cp rivendell.sys /etc/sysconfig/rivendell
  mkdir -p /var/run/rivendell
  chmod 777 /var/run/rivendell
  ldconfig
fi

chmod 4755 $1/bin/caed
chmod 4755 $1/bin/ripcd
chmod 4755 $1/bin/rdcatchd


# End of install-init.sh
