// list_reports.h
//
// List and Generate Log Reports
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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

#ifndef LIST_REPORTS_H
#define LIST_REPORTS_H

#include <qdialog.h>
#include <qdatetimeedit.h>
#include <qcombobox.h>
#include <qsqldatabase.h>

#include <rdlog.h>
#include <rdlog_event.h>

class ListReports : public QDialog
{
 Q_OBJECT
 public:
 ListReports(const QString &logname,const QString &description,
	     const QString service_name,const QDate &start_date,
	     const QDate &end_date,bool auto_refresh,RDLogEvent *events,
	     QWidget *parent=0,const char *name=0);
 ~ListReports();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;
 
 private slots:
  void selectDateData();
  void generateData();
  void closeData();

 private:
  void GenerateLogReport(QString *report);
  void GenerateExceptionReport(QString *report,const QDate &date);
  QComboBox *list_reports_box;
  QString list_log_name;
  QString list_description;
  QString list_service_name;
  QDate list_start_date;
  QDate list_end_date;
  bool list_auto_refresh;
  RDLogEvent *list_events;
  QDateEdit *list_date_edit;
};


#endif  // LIST_REPORTS_H
