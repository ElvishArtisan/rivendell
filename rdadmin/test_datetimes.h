// test_datetimes.h
//
// Test Rivendell Date/Time Formats
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

#ifndef TEST_DATETIMES_H
#define TEST_DATETIMES_H

#include <QLabel>
#include <QPushButton>

#include <rddialog.h>

class TestDatetimes : public RDDialog
{
 Q_OBJECT
 public:
  TestDatetimes(QWidget *parent=0);
  QSize sizeHint() const;

 public slots:
  int exec(const QString &long_date_fmt,const QString &short_date_fmt,
	   const QString &time_fmt);

 private slots:
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QDateTime d_sample_datetime;
  QLabel *d_sample_datetime_label;
  QLabel *d_sample_datetime_label_label;
  QLabel *d_long_date_label;
  QLabel *d_long_date_label_label;
  QLabel *d_short_date_label;
  QLabel *d_short_date_label_label;
  QLabel *d_time_label;
  QLabel *d_time_label_label;
  QPushButton *d_close_button;
};


#endif  // TEST_DATETIMES_H
