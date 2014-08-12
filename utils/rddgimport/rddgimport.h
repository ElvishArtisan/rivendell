// rddgimport.h
//
// A Qt-based application for importing Dial Global CDN downloads
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddgimport.h,v 1.1.2.4 2013/02/05 01:35:35 cvs Exp $
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

#ifndef RDDGIMPORT_H
#define RDDGIMPORT_H

#include <vector>
#include <map>

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qtextedit.h>

#include <rdconfig.h>
#include <rddb.h>
#include <rdgroup.h>
#include <rdlibrary_conf.h>
#include <rdstation.h>
#include <rdbusybar.h>
#include <rdsvc.h>
#include <rduser.h>
#include <rdripc.h>

#include <event.h>

#define RDDGIMPORT_USAGE "\n"
#define RDDGIMPORT_KILLDATE_OFFSET 7
#define RDDGIMPORT_FILE_EXTENSION "mp3"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
   MainWidget(QWidget *parent=0,const char *name=0);
   QSize sizeHint() const;

 private slots:
  void serviceActivatedData(int index);
  void filenameChangedData(const QString &str);
  void filenameSelectedData();
  void dateSelectedData();
  void processData();
  void userChangedData();
  void quitMainWidget();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void SetCaption();
  bool LoadEvents();
  bool ImportAudio();
  bool WriteTrafficFile();
  bool CheckSpot(const QString &isci);
  bool ImportSpot(Event *evt);
  void ActivateBar(bool state);
  Event *GetEvent(const QString &isci);
  QTime GetTime(const QString &str) const;
  int GetLength(const QString &str) const;
  QString GetIsci(const QString &str) const;
  void LogMessage(const QString &str);
  QLabel *dg_service_label;
  QComboBox *dg_service_box;
  QLabel *dg_filename_label;
  QLineEdit *dg_filename_edit;
  QPushButton *dg_filename_button;
  QLabel *dg_date_label;
  QDateEdit *dg_date_edit;
  QPushButton *dg_date_button;
  QLabel *dg_messages_label;
  QTextEdit *dg_messages_text;
  RDBusyBar *dg_bar;
  QPushButton *dg_process_button;
  QPushButton *dg_close_button;
  std::map<QString,unsigned> dg_carts;
  std::vector<Event *> dg_events;
  RDGroup *dg_group;
  RDSvc *dg_svc;
  RDRipc *dg_ripc;
  RDUser *dg_user;
  RDStation *dg_station;
  RDLibraryConf *dg_library_conf;
  RDConfig *dg_config;
  QSqlDatabase *dg_db;
};


#endif  // RDDGIMPORT_H
