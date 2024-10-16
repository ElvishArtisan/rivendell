// list_reports.h
//
// List and Generate RDLibrary Reports
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include <rddialog.h>

class ListReports : public RDDialog
{
  Q_OBJECT
 public:
  ListReports(const QString &filter,const QString &grpname,
	      const QString &filter_sql,int cart_limit,QWidget *parent);
  ~ListReports();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
 
 private slots:
  void typeActivatedData(int index);
  void generateData();
  void closeData();

 private:
  void GenerateCartReport(QString *report);
  void GenerateCutReport(QString *report);
  void GenerateCartDumpCsv(QString *report,bool prepend_names);
  QLabel *list_reports_label;
  QComboBox *list_reports_box;
  QCheckBox *list_fieldnames_check;
  QLabel *list_fieldnames_label;
  QString list_filter;
  QString list_type_filter;
  QString list_group;
  QString list_schedcode;
  QString list_filter_sql;
  int list_cart_limit;
};


#endif  // LIST_REPORTS_H
