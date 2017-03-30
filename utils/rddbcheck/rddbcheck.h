// rddbcheck.h
//
// A Database Check/Repair Tool for Rivendell.
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#define RDDBCHECK_USAGE "[options]\n"

//
// Global Variables
//
RDConfig *rdconfig;

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

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
  void Rehash(const QString &arg);
  void RehashCart(unsigned cartnum);
  void RehashCut(const QString &cutnum);
  void SetCutLength(const QString &cutname,int len);
  void CleanTables(const QString &ext,QSqlQuery *table_q,QSqlQuery *name_q);
  void CleanTables(const QString &ext,QSqlQuery *table_q);
  bool IsTableLinked(QSqlQuery *name_q,const QString &ext,const QString &table);
  bool UserResponse();
  bool check_yes;
  bool check_no;
  QString orphan_group_name;
  QString dump_cuts_dir;
  QString rehash;
  RDStation *check_station;
  RDUser *check_user;
};


#endif  // RDDBCHECK_H
