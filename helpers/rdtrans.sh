#!/bin/bash

# rdtrans.sh
#
# A shell utility for managing Rivendell translation files.
#
# (C) Copyright 2005,2008 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdtrans.sh,v 1.7 2010/07/29 19:32:32 cvs Exp $
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
#   The following environmental variables are used by this utility:
#
#        RD_SOURCE - The path to the top of the Rivendell source tree.
#     RD_LANGUAGES - The path to the top of the langauges tree.

#
# Some readable variable names
#
CMD=$1
LANG=$2

#
# Usage
#
USAGE="USAGE: rdtrans.sh <cmd> <lang>"


function ReadLanguage
{
    echo -n "Reading language '$LANG' ... "
    mkdir -p $RD_LANGUAGES/$LANG
    cp $RD_SOURCE/lib/librd_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdhpi/rdhpi_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdadmin/rdadmin_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdairplay/rdairplay_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdpanel/rdpanel_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdcastmanager/rdcastmanager_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdcatch/rdcatch_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdlibrary/rdlibrary_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdlogedit/rdlogedit_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdlogin/rdlogin_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/rdlogmanager/rdlogmanager_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/utils/rdgpimon/rdgpimon_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/utils/rmlsend/rmlsend_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/utils/rdchunk/rdchunk_$LANG.ts $RD_LANGUAGES/$LANG/
    cp $RD_SOURCE/utils/rdgpimon/rdgpimon_$LANG.ts $RD_LANGUAGES/$LANG/
    echo "done."
}


function WriteLanguage
{
    echo -n "Writing language '$LANG' ... "
    cp $RD_LANGUAGES/$LANG/librd_$LANG.ts $RD_SOURCE/lib/
    cp $RD_LANGUAGES/$LANG/rdhpi_$LANG.ts $RD_SOURCE/rdhpi/
    cp $RD_LANGUAGES/$LANG/rdadmin_$LANG.ts $RD_SOURCE/rdadmin/
    cp $RD_LANGUAGES/$LANG/rdairplay_$LANG.ts $RD_SOURCE/rdairplay/
    cp $RD_LANGUAGES/$LANG/rdpanel_$LANG.ts $RD_SOURCE/rdpanel/
    cp $RD_LANGUAGES/$LANG/rdcastmanager_$LANG.ts $RD_SOURCE/rdcastmanager/
    cp $RD_LANGUAGES/$LANG/rdcatch_$LANG.ts $RD_SOURCE/rdcatch/
    cp $RD_LANGUAGES/$LANG/rdlibrary_$LANG.ts $RD_SOURCE/rdlibrary/
    cp $RD_LANGUAGES/$LANG/rdlogedit_$LANG.ts $RD_SOURCE/rdlogedit/
    cp $RD_LANGUAGES/$LANG/rdlogin_$LANG.ts $RD_SOURCE/rdlogin/
    cp $RD_LANGUAGES/$LANG/rdlogmanager_$LANG.ts $RD_SOURCE/rdlogmanager/
    cp $RD_LANGUAGES/$LANG/rdgpimon_$LANG.ts $RD_SOURCE/utils/rdgpimon/
    cp $RD_LANGUAGES/$LANG/rmlsend_$LANG.ts $RD_SOURCE/utils/rmlsend/
    cp $RD_LANGUAGES/$LANG/rdchunk_$LANG.ts $RD_SOURCE/utils/rdchunk/
    cp $RD_LANGUAGES/$LANG/rdgpimon_$LANG.ts $RD_SOURCE/utils/rdgpimon/
    echo "done."
}

function PackLanguage
{
    echo $LANG > $RD_LANGUAGES/$LANG/language
    cp $RD_SOURCE/helpers/rdpack.sh $RD_LANGUAGES/$LANG
    CURRENT_DIR=`pwd`
    cd $RD_LANGUAGES/
    tar -zvcf $RD_LANGUAGES/rivendell_$LANG.tar.gz $LANG/*
    cd $CURRENT_DIR
}

function EnvironmentError
{
    echo
    echo "You need to set up the following environmental variables before"
    echo "before you can use this utility:"
    echo "  \$RD_LANGUAGES"
    echo "  \$RD_SOURCE"
    echo
}


#
# Check for a valid environment
#
if [ -z $RD_LANGUAGES ] ; then
    echo $RD_LANGUAGES
    EnvironmentError
    exit 1
fi
if [ -z $RD_SOURCE ] ; then
    echo RD_SOURCE
    EnvironmentError
    exit 1
fi

#
# Check for argument sanity
#
if [ -z $CMD ] ; then
  echo $USAGE
  exit 1
fi
if [ -z $LANG ] ; then
  echo $USAGE
  exit 1
fi

#
# dispatch the command
#
case $CMD in
    "read")
	ReadLanguage
	;;

    "write")
	WriteLanguage
	;;

    "pack")
	PackLanguage
	;;

    *)
	echo "Unknown command.  Try 'write', 'read' or 'pack'."
	exit 1
	;;
esac
