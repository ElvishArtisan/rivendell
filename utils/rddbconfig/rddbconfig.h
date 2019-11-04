// rddbconfig.h
//
// A Qt-based application to configure, backup, and restore
// the Rivendell database.
//
//   (C) Copyright 2009-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDBCONFIG_H
#define RDDBCONFIG_H

#include <qmessagebox.h>
#include <qpushbutton.h>

#include <rdwidget.h>

#include "db.h"

#define RDDBCONFIG_USAGE "\n\n";

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  ~MainWidget();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  int statusDaemons(QString service);
  void stopDaemons();
  void startDaemons();
  void createData();
  void backupData();
  void restoreData();
  void closeData();
  void mismatchData();
  void updateLabels();

 signals:
  void dbMismatch();
  void dbChanged();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDConfig *rd_config;
  bool db_manage_daemons;
  bool db_daemon_start_needed;
  QFont label_font;
  QFont day_font;
  QPushButton *db_create_button;
  QPushButton *db_backup_button;
  QPushButton *db_restore_button;
  QPushButton *db_close_button;
  QLabel *label_hostname;
  QLabel *label_username;
  QLabel *label_dbname;
  QLabel *label_schema;

  Db *db;
  bool db_open;
};

#endif  // RDDBCONFIG_H
