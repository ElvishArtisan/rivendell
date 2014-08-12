#!/bin/sh
#
# rd_mysql_enable_host
#
#  Enable a remote host in MySQL
#
# (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rd_mysql_enable_host.sh,v 1.1 2011/05/11 21:52:57 cvs Exp $
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
# Usage:
#  rd_mysql_enable_host <hostname>
#

#
# Site Defines
#
MYSQL=/usr/bin/mysql
MYSQL_ROOT_USER=root
MYSQL_ROOT_PASSWORD=
MYSQL_HOSTNAME=127.0.0.1
MYSQL_RIVENDELL_USER=rduser
MYSQL_RIVENDELL_PASSWORD=letmein
MYSQL_DBNAME=Rivendell

if [ -z $1 ] ; then
  echo "rd_mysql_enable_host <hostname>"
  exit 256
fi

if [ $MYSQL_ROOT_PASSWORD ] ; then
  MYSQL_PASSWORD_CMD=-p$MYSQL_ROOT_PASSWORD
fi

echo "insert into user set Host=\"$1\",User=\"$MYSQL_RIVENDELL_USER\",Password=password(\"$MYSQL_RIVENDELL_PASSWORD\")" | $MYSQL -h $MYSQL_HOSTNAME -u $MYSQL_ROOT_USER $MYSQL_PASSWORD_CMD mysql

echo "insert into db set Host=\"$1\",Db=\"Rivendell\",User=\"$MYSQL_RIVENDELL_USER\",Select_priv=\"Y\",Insert_priv=\"Y\",Update_priv=\"Y\",Delete_priv=\"Y\",Create_priv=\"Y\",Drop_priv=\"Y\",References_priv=\"Y\",Index_priv=\"Y\",Alter_priv=\"Y\",Create_tmp_table_priv=\"Y\",Lock_tables_priv=\"Y\",Create_view_priv=\"Y\",Show_view_priv=\"Y\",Create_routine_priv=\"Y\",Alter_routine_priv=\"Y\",Execute_priv=\"Y\"" | $MYSQL -h $MYSQL_HOSTNAME -u $MYSQL_ROOT_USER $MYSQL_PASSWORD_CMD mysql

echo "flush privileges" | $MYSQL -h $MYSQL_HOSTNAME -u $MYSQL_ROOT_USER $MYSQL_PASSWORD_CMD mysql
