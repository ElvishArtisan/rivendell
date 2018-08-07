// generate_log.h
//
// Generate a Rivendell Log
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qcombobox.h>
#include <q3datetimeedit.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <q3progressdialog.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QResizeEvent>

#include <rdnotification.h>

#define GENERATE_LOG_FILESCAN_INTERVAL 5000

class GenerateLog : public QDialog
{
 Q_OBJECT
 public:
  GenerateLog(QWidget *parent=0,int cmd_schwitch=0,QString *cmd_service=NULL,QDate *cmd_date=NULL);
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

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateControls();
  void SendNotification(RDNotification::Action action,const QString &logname);
  QLabel *gen_service_label;
  QComboBox *gen_service_box;
  QLabel *gen_date_label;
  Q3DateEdit *gen_date_edit;
  QPushButton *gen_select_button;
  QLabel *gen_import_label;
  QLabel *gen_available_label;
  QLabel *gen_merged_label;
  Q3ProgressDialog *gen_progress_dialog;
  QPushButton *gen_create_button;
  QPushButton *gen_music_button;
  QPushButton *gen_traffic_button;
  QPushButton *gen_close_button;
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

