// sas_switch_torture.h
//
// Generate Rivendell macro carts and scheduling for torture-testing
//  an SAS router
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_switch_torture.h,v 1.6 2010/07/29 19:32:39 cvs Exp $
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


#ifndef SAS_SWITCH_TORTURE_H
#define SAS_SWITCH_TORTURE_H

#include <qwidget.h>
#include <qsqldatabase.h>

#include <rdconfig.h>

#define SAS_INPUTS 32
#define SAS_OUTPUTS 16
#define SAS_STATION "hithlum"
#define SAS_MATRIX 1
#define SAS_SLEEP 20

#define CART_START 10000
#define TIME_INTERVAL 2000

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void generateData();
  void removeData();
  void cancelData();
  void closeEvent(QCloseEvent *e);

 private:
  QSqlDatabase *test_db;
  QString test_filename;
  RDConfig *rd_config;
};


#endif  // SAS_SWITCH_TORTURE_H
