#!/bin/bash

# crc-unity4k.sh
#
# Create a Unity4000 Switcher Configuration for the
# Christian Radio Consortium (CRC) Network
#
#   (C) Copyright 2004 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: crc-unity4k.sh,v 1.3 2007/02/14 21:59:12 fredg Exp $
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

# Usage: crc-unity4k <mysql-hostname> <station-name> <matrix-num>

#
# Arguments
#
HOSTNAME=$1
STATION=$2
MATRIX=$3

#
# Check Argument Sanity
#
if [ -z $HOSTNAME ] ; then
  echo "USAGE: crc-unity4k <mysql-hostname> <station-name> <marix-num>"
  exit 1
fi
if [ -z $STATION ] ; then
  echo "USAGE: crc-unity4k <mysql-hostname> <station-name> <marix-num>"
  exit 1
fi
if [ -z $MATRIX ] ; then
  echo "USAGE: crc-unity4k <mysql-hostname> <station-name> <marix-num>"
  exit 1
fi

#
# Show Settings and Confirm
#
echo
echo "This will create a set of default CRC input assignments on the following"
echo "database/matrix:"
echo "  mySQL Hostname: $HOSTNAME"
echo "  Station Name: $STATION"
echo "  Matrix Number: $MATRIX"
echo 
echo "WARNING: This will OVERWRITE any existing input assignments for"
echo "this matrix!"
echo
echo -n "Continue?"
read REPLY
echo
echo -n "Working..."

#
# Delete Existing Configuration
#
SQL="delete from INPUTS where (STATION_NAME=\"$STATION\" && MATRIX=$MATRIX)"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# Write New Inputs
#
#
# AA-1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=1,\
     NAME=\"AA-1 Stereo\",FEED_NAME=\"AA1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=2,\
     NAME=\"AA-1 Left\",FEED_NAME=\"AA1\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=3,\
     NAME=\"AA-1 Right\",FEED_NAME=\"AA1\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# AA-2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=4,\
     NAME=\"AA-2 Stereo\",FEED_NAME=\"AA2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=5,\
     NAME=\"AA-2 Left\",FEED_NAME=\"AA2\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=6,\
     NAME=\"AA-2 Right\",FEED_NAME=\"AA2\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# AMC
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=7,\
     NAME=\"AMC\",FEED_NAME=\"AMC\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# CRB
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=8,\
     NAME=\"CRB\",FEED_NAME=\"CRB\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# CSN
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=9,\
     NAME=\"CSN\",FEED_NAME=\"CSN\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# CS2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=10,\
     NAME=\"CS2\",FEED_NAME=\"CS2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# FN-1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=11,\
     NAME=\"FN-1 Stereo\",FEED_NAME=\"FN1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=12,\
     NAME=\"FN-1 Left\",FEED_NAME=\"FN1\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=13,\
     NAME=\"FN-1 Right\",FEED_NAME=\"FN1\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# FN-2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=14,\
     NAME=\"FN-2 Stereo\",FEED_NAME=\"FN2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=15,\
     NAME=\"FN-2 Left\",FEED_NAME=\"FN2\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=16,\
     NAME=\"FN-2 Right\",FEED_NAME=\"FN2\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# FN-3
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=17,\
     NAME=\"FN-3 Stereo\",FEED_NAME=\"FN3\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=18,\
     NAME=\"FN-3 Left\",FEED_NAME=\"FN3\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=19,\
     NAME=\"FN-3 Right\",FEED_NAME=\"FN3\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# FOF
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=20,\
     NAME=\"FOF\",FEED_NAME=\"FOF\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# IRN
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=21,\
     NAME=\"IRN\",FEED_NAME=\"IRN\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# LF1 Stereo
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=22,\
     NAME=\"LF1 Stereo\",FEED_NAME=\"LF1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# LF1 Left
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=23,\
     NAME=\"LF1 LEft\",FEED_NAME=\"LF1\",CHANNEL_MODE=1"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# LF1 Right
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=24,\
     NAME=\"LF1 Right\",FEED_NAME=\"LF1\",CHANNEL_MODE=2"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# MB1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=25,\
     NAME=\"MB1\",FEED_NAME=\"MB1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# MB2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=26,\
     NAME=\"MB2\",FEED_NAME=\"MB2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# REF
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=27,\
     NAME=\"REF\",FEED_NAME=\"REF\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# RR1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=28,\
     NAME=\"RR1\",FEED_NAME=\"RR1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SGT
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=29,\
     NAME=\"SGT\",FEED_NAME=\"SGT\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SKY
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=30,\
     NAME=\"SKY\",FEED_NAME=\"SKY\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SK2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=31,\
     NAME=\"SK2\",FEED_NAME=\"SK2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SMA
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=32,\
     NAME=\"SMA\",FEED_NAME=\"SMA\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SMB
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=33,\
     NAME=\"SMB\",FEED_NAME=\"SMB\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SMC
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=34,\
     NAME=\"SMC\",FEED_NAME=\"SMC\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SNN
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=35,\
     NAME=\"SNN\",FEED_NAME=\"SNN\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SOS
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=36,\
     NAME=\"SOS\",FEED_NAME=\"SOS\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SR1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=37,\
     NAME=\"SR1\",FEED_NAME=\"SR1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SR2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=38,\
     NAME=\"SR2\",FEED_NAME=\"SR2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SR3
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=39,\
     NAME=\"SR3\",FEED_NAME=\"SR3\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SR4
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=40,\
     NAME=\"SR4\",FEED_NAME=\"SR4\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# SR5
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=41,\
     NAME=\"SR5\",FEED_NAME=\"SR5\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# US1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=42,\
     NAME=\"US1\",FEED_NAME=\"US1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# US2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=43,\
     NAME=\"US2\",FEED_NAME=\"US2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# US3
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=44,\
     NAME=\"US3\",FEED_NAME=\"US3\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# US4
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=45,\
     NAME=\"US4\",FEED_NAME=\"US4\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# VC1
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=46,\
     NAME=\"VC1\",FEED_NAME=\"VC1\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# VC2
#
SQL="insert into INPUTS set STATION_NAME=\"$STATION\",MATRIX=$MATRIX,NUMBER=47,\
     NAME=\"VC2\",FEED_NAME=\"VC2\",CHANNEL_MODE=0"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

#
# Update MATRICES Table
#
SQL="update MATRICES set INPUTS=47 where (STATION_NAME=\"$STATION\" && MATRIX=$MATRIX)"
mysql Rivendell -h $HOSTNAME -u rduser -pletmein -e "$SQL"

echo "done."
echo


# End of crc-unity-4k
