#!/bin/bash

# update_icons.sh
# 
#   Update icon cache.
#
#   (C) Copyright 2021 Fred Gleason <fredg@salemradiolabs.com>
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

if test $UID = 0 ; then
  if test -x /usr/bin/gtk-update-icon-cache ; then
    /usr/bin/gtk-update-icon-cache -f /usr/share/icons/hicolor
  fi
fi

# End of update_icons.sh
