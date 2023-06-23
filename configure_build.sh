#!/bin/bash

# configure_build.sh
#
# Attempt to auto-detect the underlying Linux distribution and configure
# the build accordingly.
#
#   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

function CheckDistroType () {
    if [[ "$ID" == *"$1"* ]]; then
	return 0
    fi
    if [[ "$ID_LIKE" == *"$1"* ]]; then
	return 0
    fi
    return 1
}

function GetDistroType () {
    CheckDistroType "rhel"
    if [[ $? -eq 0 ]]; then
	DISTRO_TYPE="rhel"
	return 0
    fi

    CheckDistroType "ubuntu"
    if [[ $? -eq 0 ]]; then
        DISTRO_TYPE="ubuntu"
	return 0
    fi

    CheckDistroType "debian"
    if [[ $? -eq 0 ]]; then
        DISTRO_TYPE="debian"
	return 0
    fi
}

function GetDistroId () {
    local VERSION_ARRAY
    IFS='.' read -ra VERSION_ARRAY <<< $VERSION_ID
    DISTRO_ID=${VERSION_ARRAY[0]}
}

#
# Attempt to detect the distribution
#
. /etc/os-release

GetDistroType
GetDistroId

if [ -z $DISTRO_TYPE ]; then
    echo "unrecognized distro"
    exit 1
fi

case $DISTRO_TYPE in
    debian)
    export MUSICBRAINZ_LIBS="-ldiscid -lmusicbrainz5cc -lcoverartcc"
    CONFIGURE="./configure --prefix=/usr --libdir=/usr/lib --libexecdir=/var/www/rd-bin --sysconfdir=/etc/apache2/conf-enabled $@"
    ;;

    rhel)
    CONFIGURE="./configure --prefix=/usr --libdir=/usr/lib64 --libexecdir=/var/www/rd-bin --sysconfdir=/etc/httpd/conf.d $@"
    ;;

    ubuntu)
    export MUSICBRAINZ_LIBS="-ldiscid -lmusicbrainz5cc -lcoverartcc"
    CONFIGURE="./configure --prefix=/usr --libdir=/usr/lib --libexecdir=/var/www/rd-bin --sysconfdir=/etc/apache2/conf-enabled $@"
    ;;
esac

#
# Configure the build
#
./autogen.sh
$CONFIGURE
