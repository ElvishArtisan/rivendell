// rddatetimeedit.h
//
// QDateTimeEdit with date/time-format awareness
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDATETIMEEDIT_H
#define RDDATETIMEEDIT_H

#include <rddateedit.h>
#include <rdtimeedit.h>

class RDDateTimeEdit : public QWidget
{
  Q_OBJECT
 public:
  RDDateTimeEdit(QWidget *parent=0);
  ~RDDateTimeEdit();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QDate date() const;
  QDateTime dateTime() const;
  bool isReadOnly();
  bool showTenths() const;
  QTime time() const;

 public slots:
  void setDate(const QDate &date);
  void setDateTime(const QDateTime &dt);
  void setReadOnly(bool state);
  void setShowTenths(bool state);
  void setTime(const QTime &time);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDDateEdit *d_date_edit;
  RDTimeEdit *d_time_edit;
};


#endif  // RDDATEEDIT_H
