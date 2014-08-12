// rdlogmanager.h
//
// The Log Manager Utility for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlogmanager.h,v 1.14.6.2.2.1 2014/05/20 14:01:51 cvs Exp $
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


#ifndef RDLOGMANAGER_H
#define RDLOGMANAGER_H

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qpixmap.h>

#include <rduser.h>
#include <rdripc.h>
#include <rdlibrary_conf.h>
#include <rdlog_line.h>
#include <rdconfig.h>

#define RDLOGMANAGER_USAGE "[-P] [-g] [-m] [-t] [-r <rpt-name>] [-d <days>] [-e <days>] -s <svc-name>\n\n-P\n     Do not overwrite existing logs or imports.\n\n-g\n     Generate a new log for the specified service.\n\n-m\n     Merge the Music log for the specified service.\n\n-t\n     Merge the Traffic log for the specified service.\n\n-r <rpt-name>\n     Generate report <rpt-name>.\n\n-d <days>\n     Specify a start date offset.  For log operations, this will be added\n     to tomorrow's date to arrive at a target date, whereas for report\n     operations it will be added to yesterday's date to arrive at a target\n     date.  Default value is '0'.\n\n-e <days>\n     Specify an end date offset.  This will be added to yesterday's date\n     to arrive at a target end date.  Valid only for certain report types.\n     Default value is '0'.\n\n-s <service-name>\n     Specify service <service-name> for log operations.\n\n"

//
// Command Line Operations
//
extern int RunLogOperation(int argc,char *argv[],const QString &svcname,
			   int start_offset,bool protect_existing,bool gen_log,
			   bool merge_mus,bool merge_tfc);
extern int RunReportOperation(int argc,char *argv[],const QString &rptname,
			      bool protect_existing,int start_offset,
			      int end_offset);


class MainWidget : public QWidget
{
 Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void userData();
  void eventsData();
  void clocksData();
  void gridsData();
  void generateData();
  void reportsData();
  void quitMainWidget();

 private:
  void LoadConfig();
  QSqlDatabase *log_db;
  QString log_filename;
  QLabel *log_user_label;
  QPushButton *log_events_button;
  QPushButton *log_clocks_button;
  QPushButton *log_grids_button;
  QPushButton *log_logs_button;
  QPushButton *log_reports_button;
  QPushButton *log_close_button;
  QPixmap *log_rivendell_map;
};


#endif 
