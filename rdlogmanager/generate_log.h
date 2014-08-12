// generate_log.h
//
// Generate a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: generate_log.h,v 1.9 2010/07/29 19:32:37 cvs Exp $
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

#ifndef GENERATE_LOG_H
#define GENERATE_LOG_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qprogressdialog.h>
#include <qpixmap.h>

#define GENERATE_LOG_FILESCAN_INTERVAL 5000

class GenerateLog : public QDialog
{
 Q_OBJECT
 public:
  GenerateLog(QWidget *parent=0,const char *name=0,int cmd_schwitch=0,QString *cmd_service=NULL,QDate *cmd_date=NULL);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void serviceActivatedData(int index);
  void dateChangedData(const QDate &date);
  void selectDateData();
  void createData();
  void musicData();
  void trafficData();
  void fileScanData();
  void closeData();

 private:
  void UpdateControls();
  QComboBox *gen_service_box;
  QDateEdit *gen_date_edit;
  QProgressDialog *gen_progress_dialog;
  QPushButton *gen_create_button;
  QPushButton *gen_music_button;
  QPushButton *gen_traffic_button;
  QLabel *gen_tfc_avail_label;
  QLabel *gen_tfc_merged_label;
  QLabel *gen_mus_avail_label;
  QLabel *gen_mus_merged_label;
  QPixmap *gen_whiteball_map;
  QPixmap *gen_greenball_map;
  QPixmap *gen_redball_map;
  bool gen_music_enabled;
  bool gen_traffic_enabled;
  int cmdswitch;
  QString *cmdservice;
  QDate *cmddate;
};


#endif

