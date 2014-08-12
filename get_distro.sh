#!/bin/bash

# get_distro.sh
# 
# Try to determine the distribution name and version of the host machine.
# Used as part of the AR_GET_DISTRO() macro.
#
#   (C) Copyright 2007 Fred Gleason <fredg@salemradiolabs.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of
#   the License, or (at your option) any later version.
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

case "$1" in
    NAME)
	if test -f /etc/SuSE-release ; then
	  echo -n "SuSE"
	  exit 0
	fi
	if test -f /etc/debian_version ; then
	  echo -n "Debian"
	  exit 0
	fi
	if test -f /etc/redhat-release ; then
	  echo -n "RedHat"
	  exit 0
	fi
	echo -n "unknown"
    ;;
    VERSION)
	if test -f /etc/SuSE-release ; then
          cat /etc/SuSE-release | sed "/SE/ d;s/VERSION = //"
	  exit 0
        fi
	if test -f /etc/debian_version ; then
          cat /etc/debian_version
	  exit 0
        fi
	if test -f /etc/redhat-release ; then
	  awk '/release/ {print $3}' /etc/redhat-release
	fi
    ;;
    MAJOR)
	if test -f /etc/SuSE-release ; then
          cat /etc/SuSE-release | sed "/SE/ d;s/VERSION = //" | awk -F '.' '{print $1}'
	  exit 0
        fi
	if test -f /etc/debian_version ; then
          cat /etc/debian_version | awk -F '.' '{print $1}'
	  exit 0
        fi
	if test -f /etc/redhat-release ; then
	  awk '/release/ {print $3}' /etc/redhat-release | awk -F '.' '{print $1}'
	fi
    ;;
    MINOR)
	if test -f /etc/SuSE-release ; then
          cat /etc/SuSE-release | sed "/SE/ d;s/VERSION = //" | awk -F '.' '{print $2}'
	  exit 0
        fi
	if test -f /etc/debian_version ; then
          cat /etc/debian_version | awk -F '.' '{print $2}'
	  exit 0
        fi
	if test -f /etc/redhat-release ; then
	  awk '/release/ {print $3}' /etc/redhat-release | awk -F '.' '{print $2}'
	fi
    ;;
esac


# End of get_distro.sh
