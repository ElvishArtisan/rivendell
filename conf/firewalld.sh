#!/bin/bash

# firewalld.sh
#
#  Control firewalld
#
# (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

OPERATION=$1

if test $UID -eq 0 ; then
    if test -z $FAKEROOTKEY ; then
	if test $OPERATION = "reload" ; then
	    /bin/systemctl reload firewalld
	fi
	if test $OPERATION = "add" ; then
	    /bin/firewall-cmd --zone=public --add-service=rivendell --permanent
	    /bin/firewall-cmd --zone=work --add-service=rivendell --permanent
	    /bin/firewall-cmd --zone=home --add-service=rivendell --permanent
	fi
	if test $OPERATION = "remove" ; then
	    /bin/firewall-cmd --zone=public --remove-service=rivendell --permanent
	    /bin/firewall-cmd --zone=work --remove-service=rivendell --permanent
	    /bin/firewall-cmd --zone=home --remove-service=rivendell --permanent
	fi
    fi
fi
