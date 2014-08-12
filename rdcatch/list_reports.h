// list_reports.h
//
// List and Generate RDCatch Reports
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.h,v 1.5 2010/07/29 19:32:36 cvs Exp $
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
#include <qcombobox.h>
#include <qsqldatabase.h>

#include <rdsvc.h>


class ListReports : public QDialog
{
 Q_OBJECT
 public:
 ListReports(bool active_only,bool today_only,int dow,
	     QWidget *parent=0,const char *name=0);
 ~ListReports();
 QSize sizeHint() const;
 QSizePolicy sizePolicy() const;
 
 private slots:
  void generateData();
  void closeData();

 private:
  void GenerateEventReport(QString *report);
  void GenerateXloadReport(QString *report);
  QString GetSourceName(const QString &station,int matrix,int input);
  QString GetDestinationName(const QString &station,int matrix,int output);
  QComboBox *list_reports_box;
  bool list_active_only;
  bool list_today_only;
  int list_dow;
};


#endif  // LIST_REPORTS_H
