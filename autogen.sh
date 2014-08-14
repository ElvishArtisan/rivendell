#!/bin/sh
##
##    (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: autogen.sh,v 1.5 2010/07/29 19:32:30 cvs Exp $
##
##    Adapted from './autogen.sh' in the Jack Audio Connection Kit.
##    Copyright (C) 2001-2003 Paul Davis, et al.
##
##    This program is free software; you can redistribute it and/or modify
##    it under the terms of version 2 of the GNU General Public License as
##    published by the Free Software Foundation;
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program; if not, write to the Free Software
##    Foundation, Inc., 59 Temple Place, Suite 330, 
##    Boston, MA  02111-1307  USA
##

libtoolize=libtoolize
if which glibtoolize > /dev/null 2>&1; then
      libtoolize=glibtoolize
fi

$libtoolize --force 2>&1 | sed '/^You should/d' || {
    echo "libtool failed, exiting..."
    exit 1
}

aclocal $ACLOCAL_FLAGS || {
    echo "aclocal \$ACLOCAL_FLAGS where \$ACLOCAL_FLAGS= failed, exiting..."
    exit 1
}

automake --add-missing -Wno-portability || {
    echo "automake --add-missing failed, exiting..."
    exit 1
}

autoconf || {
    echo "autoconf failed, exiting..."
    exit 1
}
