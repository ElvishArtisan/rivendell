#! /bin/bash

# rd_restore_system.sh
#
# Restore a complete Rivendell setup from removable media.
#
#   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rd_restore_system.sh,v 1.2 2011/05/23 11:22:48 cvs Exp $
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
# This script is intended for use as a starting point for a site-specific
# backup routine.  Administrators are strongly encouraged to customize it
# for their envirnment!
#

#
# Site Configuration
#
MYSQL_USERNAME=rduser
MYSQL_PASSWORD=letmein
MYSQL_HOSTNAME=127.0.0.1
MYSQL_DBNAME=Rivendell
BACKUP_DEVICE=/dev/sdb1
BACKUP_MOUNT_POINT=/media/rdbackup

function Continue {
  read -a RESP -p "Continue (y/N) "
  echo
  if [ -z $RESP ] ; then
    exit 0
  fi
  if [ $RESP != "y" -a $RESP != "Y" ] ; then
    exit 0
  fi
}

#
# Warning Messages
#
echo "This process will restore the Rivendell system from external media."
echo
echo "WARNING: This will COMPLETELY OVERWRITE the data on the Rivendell system!"
echo
Continue

echo "Please verify that the external drive is connected before continuing."
echo
Continue

#
# Setup environment
#
echo -n "Preparing backup device..."
sleep 5
mkdir -p $BACKUP_MOUNT_POINT
mount $BACKUP_DEVICE $BACKUP_MOUNT_POINT
mkdir -p $BACKUP_MOUNT_POINT/
echo "done."


#
# Restore Database
#
echo -n "Restoring database..."
echo "drop database $MYSQL_DBNAME" | mysql -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME -p$MYSQL_PASSWORD
echo "create database $MYSQL_DBNAME" | mysql -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME -p$MYSQL_PASSWORD
cat $BACKUP_MOUNT_POINT/db.sql | mysql -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME -p$MYSQL_PASSWORD $MYSQL_DBNAME
echo "done."

#
# Restore Audio
#
echo -n "Restoring audio..."
rsync -av --delete $BACKUP_MOUNT_POINT/snd/ /var/snd
echo "done."

#
# Clean Up
#
umount $BACKUP_MOUNT_POINT
rmdir $BACKUP_MOUNT_POINT
/etc/init.d/rivendell restart
echo
echo "Restore complete!"
echo

exit 0
