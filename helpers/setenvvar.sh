#!/bin/bash
#
# setenvvar.sh
#
# A utility for setting environmental variables to be used by the
# rdtrans.sh script that is used to manage Rivendell translation files.
#
# (C) Copyright 2010 Frederick Henderson <frederickjh@henderson-meier.org>
#
#      $Id: setenvvar.sh,v 1.1 2011/03/01 21:00:07 cvs Exp $
#      $Date: 2011/03/01 21:00:07 $
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
#   The following environmental variables are set by this utility:
#
#        RD_SOURCE - The path to the top of the Rivendell source tree.
#     RD_LANGUAGES - The path to the top of the languages tree.
#                    This a working folder for your translation files.
#     RD_SHAREDDIR - The path to Rivendell's shared data directory
#                    /usr/local/share/rivendell by default
#                    /usr/share/rivendell on Ubuntu
#
## IMPORTANT NOTE: This script must be run in a special way. The best
## is if you run it from the /helpers folder with the following command:
## . ./setenvvar.sh
## If the first .(period) and space are omitted the script will run but
## environmental variables will not be set.
#
#
## Functions

function whichdialog()
{
zenitypresent="$(type "zenity" 2> /dev/null )"
# Line below commented out. Used for testing kdialog
#zenitypresent=
kdialogpresent="$(type "kdialog" 2> /dev/null )"
# Line below commented out. Used for testing bash 4
#kdialogpresent=
if [ ${#zenitypresent} -gt 0 ] ; then
   dialog=zenity
else
   if [ ${#kdialogpresent} -gt 0 ] ; then
      dialog=kdialog
   else
     if [ $BASH_VERSINFO -lt 4 ] ; then 
       echo "Sorry this script need at least Bash version 4 to run correctly."
       echo "Try installing 'zenity' or 'kdialog' from the 'kdebase-bin'\
 package."
       exit
     fi
      dialog=read
   fi
fi
}

function getdir()
{
	if [ "${dialog}" = "kdialog" ] ; then
           ${dialog} --title "$shorttitle" --passivepopup "$title" 15
           height=600 ;  width=1050
           selecteddir=$(${dialog} --title "$title"  --getexistingdirectory\
           "${dir}" --geometry ${width}x${height}+0-0 2> /dev/null )
           if [ $? -ne 0 ] ; then      
		kill -SIGINT $$
           fi
	elif [ "${dialog}" = "zenity" ] ; then
                ${dialog} --info --title "$shorttitle" --text "$title"\
                --timeout=15
		selecteddir=$(${dialog} --title "$title" --file-selection\
                --directory --filename="$dir/" 2> /dev/null )
           if [ $? -ne 0 ] ; then      
		kill -SIGINT $$
           fi
        elif [ "${dialog}" = "read" ]; then
                echo ======================================================\
=====================================
                echo
                read -e -p "$title "  -i "$dir" selecteddir
                echo
                echo =======================================================\
===================================
	fi 
}

function setrdsource()
{
dir=${PWD%\/*}
title="Enter the directory for the RD_SOURCE variable. If you ran this in the\
 /helpers directory then you can most likely just use the default of $dir ."
shorttitle="Enter the directory for the RD_SOURCE variable."
if [ -n "$RD_SOURCE" ] ; then
  dir="$RD_SOURCE"
fi
getdir
export RD_SOURCE="$selecteddir"
}

function setrdlanguages()
{
dir="$HOME/rdlanguages"
title="Enter the directory for the RD_LANGUAGES variable. This will be your\
 working folder for your translation files. The default is $dir in your home\
 folder."
shorttitle="Enter the directory for the RD_LANGUAGES variable."
if [ -n "$RD_LANGUAGES" ] ; then
  dir="$RD_LANGUAGES"
fi
getdir
export RD_LANGUAGES="$selecteddir"
}

function setrdshareddir()
{
title="Enter the directory for the RD_SHAREDDIR variable. For Ubuntu use\
 /usr/share/rivendell The default is /usr/local/share/rivendell"
shorttitle="Enter the directory for the RD_SHAREDDIR variable."
if [ -n "$RD_SHAREDDIR" ] ; then
  dir="$RD_SHAREDDIR"
else
  distro=$(cat /etc/*_ver* /etc/*-rel* | sed '/^DISTRIB_ID=*/!d; s///; q')
  if [ $distro = "Ubuntu" ] ; then
    dir="/usr/share/rivendell"
  else
    dir="/usr/local/share/rivendell"
  fi
fi
getdir
export RD_SHAREDDIR="$selecteddir"
}

function addtodotprofile()
{
title="Would you like me to add these variables to your $HOME/.profile file,\
 so that they are loaded each time you login?"
shorttitle="Would you like  me to add . . ."
	if [ "${dialog}" = "kdialog" ] ; then
           ${dialog} --title "$shorttitle" --yesno "$title"
           if [ $? -ne 0 ] ; then      
		kill -SIGINT $$
           else
               envtoprofile
           fi
	elif [ "${dialog}" = "zenity" ] 
		then
                ${dialog} --question --title "$shorttitle" --text "$title"
           if [ $? -eq 0 ] ; then      
	     envtoprofile
           fi
        elif [ "${dialog}" = "read" ]; then
                echo =====================================================\
=====================================
                echo
                answer=
                until [[ "$answer" = "Yes" || "$answer" = "yes" ||\
                "$answer" = "No" || "$answer" = "no" ]] ; do
                  read -e -p "$title "  -i "Yes" answer
		  if [[ "$answer" = "Yes" || "$answer" = "yes" ]] ; then
                    envtoprofile
                  fi
                done
                echo
                echo =====================================================\
=====================================
	fi 
}

function envtoprofile()
{
echo >> $HOME/.profile
echo "### The lines below were added to set Environmental Variables for\
 Rivendell translation script rdtrans.sh" >> $HOME/.profile
echo 'export RD_SOURCE='"$RD_SOURCE" >> $HOME/.profile
echo 'export RD_LANGUAGES='"$RD_LANGUAGES" >> $HOME/.profile
echo 'export RD_SHAREDDIR='"$RD_SHAREDDIR" >> $HOME/.profile
echo "### End of environmental variables for rdtrans.sh" >> $HOME/.profile
echo >> $HOME/.profile
}

## MAIN PROGRAM ###
whichdialog
setrdsource
setrdlanguages
setrdshareddir
addtodotprofile
