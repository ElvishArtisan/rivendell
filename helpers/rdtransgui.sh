#!/bin/bash
#
# rdtransgui.sh
#
# A GUi interface for rdtrans.sh script that is used to manage Rivendell
# translation files.
#
# (C) Copyright 2010 Frederick Henderson <frederickjh@henderson-meier.org>
#
#      $Id: rdtransgui.sh,v 1.1 2011/03/01 21:00:07 cvs Exp $
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
### Functions ###
function whichdialog()
{
zenitypresent="$(type "zenity" 2> /dev/null )"
# Line below commented out. Used for testing kdialog
#zenitypresent=
kdialogpresent="$(type "kdialog" 2> /dev/null )"
if [ ${#zenitypresent} -gt 0 ] ; then
   dialog=zenity
else
   if [ ${#kdialogpresent} -gt 0 ] ; then
      dialog=kdialog
   else
     echo "Sorry this script needs a script GUI to run correctly."
     echo "Try installing 'zenity' or 'kdialog' from the 'kdebase-bin'\
 package."
     exit

   fi
fi
}

# Check for a valid environment
#
function checkenviroment()
{
if [[ -z "$RD_LANGUAGES" || -z "$RD_SOURCE" || -z "$RD_SHAREDDIR" ]] ; then
  text="First you need to set the Enviromental Variables for this script to\
 work. I will bring up the setenvvar.sh script so you can do so before\
 continuing to the menu."
  title="Need to set Enviromental Variables first."
  if [ "${dialog}" = "kdialog" ] ; then
     ${dialog} --title "$title" --passivepopup "$text" 15
  elif [ "${dialog}" = "zenity" ] ; then
    ${dialog} --info --title="$title" --text="$text"\
    --timeout=15
  fi
    . ./setenvvar.sh
fi
}

function setlanguagecode()
{
title="Enter a Valid Language Code" 
text="Enter a Valid i18n language code. These are two letter codes for\
 language.
 ie. German=de 

These maybe followed by an underscore and a capitalize two letter country code.
ie. de__CH for German and Switzerland to create a country specific language\
 code.

You can find a list in '''/usr/share/i18n/SUPPORTED''' on Ubuntu systems.

Run 'man locale' and check near the bottom of the man page for the location on\
 your system
of the SUPPORTED locale codes file. Only the portion before .UTF-8 is needed. 

The locale code that your computer is set to now is filled in below for you."
computerlocale=$(echo $LANG | sed 's/.utf8$//')
  if [ "${dialog}" = "kdialog" ] ; then
     langcode=$(${dialog} --title "$title" --inputbox "$text"\
 "$computerlocale")
  elif [ "${dialog}" = "zenity" ] ; then
    langcode=$(${dialog} --entry --title="$title" --text="$text"\
     --entry-text="$computerlocale")
  fi
checkforcode
}

function checkforcode()
{
until [ -n "$langcode" ] ; do
setlanguagecode
done
}

function menu()
{
while true; do
title="RD Translation Utility - $langcode"
text="Please choose from the menu."
 if [ "${dialog}" = "kdialog" ] ; then
    choice=$(${dialog} --title "$title" --menu "$text" "Add Language"\
 "Add Language" "Remove Language" "Remove Language" "Update Language"\
 "Update Language" "Read Language" "Read Language" "Write Language"\
 "Write Language" "Test Language" "Test Language" "Pack Language"\
 "Pack Language" "Set Environmental Variables" "Set Environmental Variables"\
 "Set Language Code" "Set Language Code" "Exit" "Exit" )
  elif [ "${dialog}" = "zenity" ] ; then
 choice=$(zenity --width=350 --height=350 --list --column "" --title="$title"\
 --text="$text" "Add Language" "Remove Language" "Update Language"\
 "Read Language" "Write Language" "Test Language" "Pack Language"\
 "Set Environmental Variables"  "Set Language Code" "Exit")
  fi


  case "${choice}" in

"Add Language")
./rdtrans.sh add $langcode
;;
"Remove Language")
./rdtrans.sh remove $langcode
;;
"Update Language")
./rdtrans.sh update $langcode
;;
"Read Language")
./rdtrans.sh read $langcode
;;
"Write Language")
./rdtrans.sh write $langcode
;;
"Test Language")
title="Password needed!"
text="Please enter your sudo password
 as the test command needs super user
 privileges."
if [ "${dialog}" = "kdialog" ] ; then
    password=$(${dialog} --title "$title" --password "$text")
    echo $password | sudo -ES ./rdtrans.sh test $langcode
    password=
  elif [ "${dialog}" = "zenity" ] ; then
    password=$(${dialog} --title="$title" --text="$text" --entry --hide-text)
    echo $password | sudo -ES ./rdtrans.sh test $langcode
    password=
    sudo -k
  fi

;;
"Pack Language")
. ./rdtrans.sh pack $langcode
title="Archive is ready to email!"
text="All done. Archive for language $LANG can be found here:
$RD_LANGUAGES/rivendell_$LANG.tar.gz
Please email it to the developer - $MAIL_ADDR
and thanks for translating Rivendell!"
if [ "${dialog}" = "kdialog" ] ; then
     ${dialog} --title "$title" --msgbox "$text"
elif [ "${dialog}" = "zenity" ] ; then
    ${dialog} --info --title="$title" --text="$text"
fi
;;
"Set Environmental Variables")
. ./setenvvar.sh
;;
"Set Language Code")
setlanguagecode
;;
Exit|*)
      break
    ;;
  esac
done
}

### Main Program ###
whichdialog
checkenviroment
setlanguagecode
menu
