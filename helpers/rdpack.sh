#!/bin/bash

# rdpack.sh
#
# A shell utility for packaging Rivendell translation files.
#
# (C) Copyright 2005,2008 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdpack.sh,v 1.6 2010/07/29 19:32:32 cvs Exp $
#      $Date: 2010/07/29 19:32:32 $
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

MAIL_ADDR=translations@paravelsystems.com


echo -n "Packaging translation files..."

#
# Get Language
#
LANG=`cat language`

#
# Package it
#
tar zvcf rivendell_$LANG-done.tar.gz * > /dev/null

echo "done."
echo
echo "Please e-mail the file 'rivendell_$LANG-done.tar.gz' to"
echo "'$MAIL_ADDR.'"
echo
echo "Thank you for supporting the Rivendell project!"
echo


# End of rdpack.sh
