// rdcheckcuts.h
//
// Check Rivendell Cuts for Valid Audio
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcheckcuts.h,v 1.1.2.1 2012/10/13 04:53:19 cvs Exp $
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


#ifndef RDCHECKCUTS_H
#define RDCHECKCUTS_H

#include <vector>

#include <qobject.h>

#include <rdconfig.h>

#define RDCHECKCUTS_USAGE "[options]\n\nCheck Rivendell cuts for valid audio\n\n--group=<group-name>\n     Name of group to scan.  This option may be given multiple times.\n     If no group is specified, then ALL groups will be scanned.\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void RenderCut(const QString &cutname);
  bool ValidateGroup(const QString &groupname,std::vector<QString> *cutnames);
  bool OpenDb();
  void CloseDb();
  RDConfig *cut_config;
};


#endif  // RDCHECKCUTS_H
