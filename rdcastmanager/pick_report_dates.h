// pick_report_date.h
//
// Select a Set of Dates for a Rivendell Podcast Report
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pick_report_dates.h,v 1.3 2010/07/29 19:32:36 cvs Exp $
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
  PickReportDates(unsigned feed_id,unsigned cast_id,
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
  void GenerateSubscriptionReport(const QString &keyname,QString *rpt);
  void GenerateEpisodeReport(const QString &keyname,unsigned cast_id,
			     QString *rpt);
  QDateEdit *edit_startdate_edit;
  QDateEdit *edit_enddate_edit;
  QString edit_keyname;
  unsigned edit_feed_id;
  unsigned edit_cast_id;
};


#endif  // PICK_REPORT_DATES_H
