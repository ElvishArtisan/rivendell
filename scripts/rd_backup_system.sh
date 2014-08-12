#! /bin/bash

# rd_backup_system.sh
#
# Back up a complete Rivendell setup to removable media.
#
#   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rd_backup_system.sh,v 1.3 2011/05/23 11:22:48 cvs Exp $
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
# Calculate date/time string
#
DATE_STAMP=`date +%Y%m%d%H%M%S`

#
# Warning Messages
#
echo "This process will backup the Rivendell system to external media."
echo
echo "WARNING: This will completely destroy any data on the external media!"
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
rm -f $BACKUP_MOUNT_POINT/*.sql
mkdir -p $BACKUP_MOUNT_POINT/snd
echo "done."


#
# Backup Database
#
echo -n "Backing up database..."
mysqldump -h $MYSQL_HOSTNAME -u $MYSQL_USERNAME -p$MYSQL_PASSWORD $MYSQL_DBNAME > $BACKUP_MOUNT_POINT/db-$DATE_STAMP.sql
ln -s db-$DATE_STAMP.sql $BACKUP_MOUNT_POINT/db.sql
echo "done."

#
# Backup Audio
#
echo -n "Backup up audio..."
rsync -av --delete /var/snd/ $BACKUP_MOUNT_POINT/snd
echo "done."

#
# Clean Up
#
umount $BACKUP_MOUNT_POINT
rmdir $BACKUP_MOUNT_POINT
echo
echo "Backup complete!"
echo

exit 0
