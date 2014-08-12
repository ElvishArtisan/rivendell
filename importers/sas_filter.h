// sas_filter.h
//
// An RDCatch event import filter for the SAS64000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_filter.h,v 1.7 2010/07/29 19:32:32 cvs Exp $
//      $Date: 2010/07/29 19:32:32 $
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

#ifndef SAS_FILTER_H
#define SAS_FILTER_H

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#include <rdstation.h>
#include <rdripc.h>
#include <rdcatch_connect.h>
#include <rdstation.h>
#include <rdconfig.h>

#define SAS_FILTER_USAGE "-d|-i <insert-list>\n"

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void InsertList();
  void DeleteList();
  void InjectLine(char *line);
  void InjectSwitchEvent(QString sql,int input,int output);
  void InjectCartEvent(QString sql,int gpo);
  RDStation *filter_rdstation;
  RDRipc *filter_ripc;
  RDCatchConnect *filter_connect;
  QSqlDatabase *filter_db;
  int filter_switch_count;
  int filter_macro_count;
  RDConfig *rd_config;
};


#endif 
