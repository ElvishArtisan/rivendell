// rdadmin.h
//
// The Administration Utility for Rivendell.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef RDADMIN_H
#define RDADMIN_H

#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qpixmap.h>

#include <rdwidget.h>

#define RDADMIN_USAGE "\n"

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *config,RDWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void manageUsersData();
  void manageGroupsData();
  void manageServicesData();
  void manageStationsData();
  void systemSettingsData();
  void reportsData();
  void podcastsData();
  void quitMainWidget();
  void manageSchedCodes();
  void manageReplicatorsData();
  void manageNexusData();
  void systemInfoData();
  
 private:
  void ClearTables();
  QString admin_username;
  QString admin_password;
  QPixmap *admin_rivendell_map;
  QString admin_filter;
  QString admin_group;
  QString admin_schedcode;
};


#endif  // RDADMIN_H
