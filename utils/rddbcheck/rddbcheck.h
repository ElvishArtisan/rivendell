// rddbcheck.h
//
// A Database Check/Repair Tool for Rivendell.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddbcheck.h,v 1.10.6.1 2014/06/02 18:52:22 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//


#ifndef RDDBCHECK_H
#define RDDBCHECK_H

#include <list>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rduser.h>
#include <rdconfig.h>
#include <rdcmd_switch.cpp>
#include <rdstation.h>

#define RDDBCHECK_USAGE "[options]\n\nCheck the Rivendell database and audio store for consistency\nand correctness.\n\n--yes\n     Answer all questions with 'yes'\n\n--no\n     Answer all questions with 'no'\n\n--user=<username>\n     Connect using the Rivendell user <username> (default is \"user\").\n\n--orphan-group=<group-name>\n     Move carts with missing/invalid GROUP information to the <group-name>\n     group.\n\n--dump-cuts-dir=<dir-name>\n     Move orphaned cut audio to the <dir-name> directory.\n"

//
// Global Variables
//
RDConfig *rdconfig;


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void CheckOrphanedTracks();
  void CheckClocks();
  void CheckEvents();
  void CheckOrphanedTables();
  void CheckCutCounts();
  void CheckPendingCarts();
  void CheckOrphanedCarts();
  void CheckOrphanedCuts();
  void CheckOrphanedAudio();
  void ValidateAudioLengths();
  void SetCutLength(const QString &cutname,int len);
  void CleanTables(const QString &ext,QSqlQuery *table_q,QSqlQuery *name_q);
  void CleanTables(const QString &ext,QSqlQuery *table_q);
  bool IsTableLinked(QSqlQuery *name_q,const QString &ext,const QString &table);
  bool UserResponse();
  bool check_yes;
  bool check_no;
  QString orphan_group_name;
  QString dump_cuts_dir;
  RDStation *check_station;
  RDUser *check_user;
};


#endif  // RDDBCHECK_H
