#!/bin/sh

# rdmemcheck.sh
#
# Script for monitoring Rivendell memory usage.
#
# (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdmemcheck.sh,v 1.1 2010/12/21 17:28:43 cvs Exp $
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

USAGE="rdmemcheck.sh <module>"

if [ -z $1 ] ; then
  echo $USAGE
  exit 256;
fi
MODULE=$1

#
# Datestamp
#
echo `date +"%m-%d-%Y %H:%M:%S"` $LOGFILE

#
# Module
#
echo -n "$MODULE -" $LOGFILE
echo -n " vsz:"
echo -n `ps --no-headers -C $MODULE -o vsz`
echo -n " rss:"
echo -n `ps --no-headers -C $MODULE -o rss`
echo

#
# rdcatchd(8)
#
echo -n "rdcatchd -" $LOGFILE
echo -n " vsz:"
echo -n `ps --no-headers -C rdcatchd -o vsz`
echo -n " rss:"
echo -n `ps --no-headers -C rdcatchd -o rss`
echo

#
# ripcd(8)
#
echo -n "ripcd -" $LOGFILE
echo -n " vsz:"
echo -n `ps --no-headers -C ripcd -o vsz`
echo -n " rss:"
echo -n `ps --no-headers -C ripcd -o rss`
echo

#
# caed(8)
#
echo -n "caed -" $LOGFILE
echo -n " vsz:"
echo -n `ps --no-headers -C caed -o vsz`
echo -n " rss:"
echo -n `ps --no-headers -C caed -o rss`
echo

echo

#echo $MODULE: $PID
