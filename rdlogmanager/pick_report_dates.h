// pick_report_date.h
//
// Select a Set of Dates for a Rivendell Report
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pick_report_dates.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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

#ifndef PICK_REPORT_DATE_H
#define PICK_REPORT_DATE_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>


class PickReportDates : public QDialog
{
  Q_OBJECT
 public:
  PickReportDates(const QString &svcname,
		    QWidget *parent=0,const char *name=0);
  ~PickReportDates();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void selectStartDateData();
  void selectEndDateData();
  void generateData();
  void closeData();

 private:
  QComboBox *edit_report_box;
  QDateEdit *edit_startdate_edit;
  QDateEdit *edit_enddate_edit;
  QString edit_svcname;
};


#endif  // PIC_REPORT_DATES_H
