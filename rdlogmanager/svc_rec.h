// svc_rec.h
//
// A Calendar Widget.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: svc_rec.h,v 1.5 2010/07/29 19:32:37 cvs Exp $
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


#ifndef SVC_REC_H
#define SVC_REC_H

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>

//
// Display Settings
//
#define SVC_REC_X_ORIGIN 20
#define SVC_REC_X_INTERVAL 25
#define SVC_REC_Y_ORIGIN 30
#define SVC_REC_Y_INTERVAL 20

class SvcRec : public QWidget
{
  Q_OBJECT
 public:
  SvcRec(const QString &svcname,QWidget *parent=0,const char *name=0);
  ~SvcRec();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QString serviceName() const;
  QDate date() const;
  bool setDate(QDate date);
  bool dayActive(int day) const;
  void deleteDay();

 signals:
  void dateSelected(const QDate &date,bool active);

 private slots:
  void monthActivatedData(int id);
  void yearActivatedData(int id);
  void yearChangedData(int year);

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  void PrintDays();
  void PrintDay(int day,int dow_offset);
  void SelectDay(int day,int dow_offset,bool state);
  void GetActiveDays(const QDate &date);
  QComboBox *pick_month_box;
  QComboBox *pick_year_box;
  QSpinBox *pick_year_spin;
  QLabel *pick_date_label[6][7];
  QDate pick_date;
  QString pick_tablename;
  int pick_low_year;
  int pick_high_year;
  bool pick_active_days[31];
  QFont pick_day_font[2];
  QString pick_service_name;
};


#endif  // SVC_REC_H
