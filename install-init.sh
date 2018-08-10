#! /bin/bash

# install-init.sh
#
# Install the init scripts for Rivendell
#
# (C) Copyright 2005,2016 Fred Gleason <fredg@paravelsystems.com>
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
  mkdir -p $2/etc/init.d
  cp rivendell $2/etc/init.d/rivendell
  cp rdrepld-suse $2/etc/init.d/rdrepld
  mkdir -p $2/etc/sysconfig
  cp rivendell.sys $2/etc/sysconfig/rivendell
  mkdir -p $2/var/run/rivendell
  if test -z $2 ; then
    chmod 777 /var/run/rivendell
    ldconfig
  fi
fi

# End of install-init.sh
