// rdmarkerset.h
//
// Command-line tool for setting Rivendell Cut Markers
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmarkerset.h,v 1.1.2.1 2014/01/16 02:45:00 cvs Exp $
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


#ifndef RDMARKERSET_H
#define RDMARKERSET_H

#include <stdlib.h>

#include <vector>

#include <qobject.h>
#include <qsqldatabase.h>

#include <rdripc.h>
#include <rduser.h>
#include <rdconfig.h>
#include <rdcmd_switch.h>
#include <rdstation.h>

#define RDMARKERSET_USAGE "[options]\nThe following options are recognized:\n\n--group=<group>\n     Apply marker changes to the group specified by <group>.  This\n     option may be given multiple times.\n\n--all-groups\n     Apply marker changes to ALL groups.\n\n--auto-trim=<level>\n     Auto-trim the specified cuts to the level indicated by <level> dBFS.\n     Specifying a '0' for <level> will remove auto-trim --i.e. move the\n     Start and End markers to the extreme start and end of the audio data.\n\n--verbose\n     Print messages to stdout describing progress.\n\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void userChangedData();

 private:
  void ProcessGroup(const QString &group_name);
  void SetAutoTrim(unsigned cartnum,int cutnum,const QString &title,
		   const QString &desc);
  void ClearAutoTrim(unsigned cartnum,int cutnum,const QString &title,
		     const QString &desc);
  void Print(const QString &msg);
  bool set_all_groups;
  std::vector<QString> set_group_names;
  int set_auto_trim;
  bool set_verbose;
  RDStation *set_station;
  RDRipc *set_ripc;
  RDUser *set_user;
  RDConfig *set_config;
};


#endif  // RDMARKERSET_H
