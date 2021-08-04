// test_datetimes.cpp
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

#include "test_datetimes.h"

TestDatetimes::TestDatetimes(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDAdmin - "+tr("Test Date/Time Formats"));

  d_sample_datetime=QDateTime(QDate(2021,12,28),QTime(14,10,23));

  d_sample_datetime_label_label=new QLabel(tr("Sample Date/Time")+":",this);
  d_sample_datetime_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_sample_datetime_label_label->setFont(labelFont());
  d_sample_datetime_label=new QLabel(this);
  d_sample_datetime_label->
    setText(d_sample_datetime.toString("dddd MMMM d yyyy - h:mm:ss AP"));

  d_long_date_label_label=new QLabel(tr("Long Date")+":",this);
  d_long_date_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_long_date_label_label->setFont(labelFont());
  d_long_date_label=new QLabel(this);

  d_short_date_label_label=new QLabel(tr("Short Date")+":",this);
  d_short_date_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_short_date_label_label->setFont(labelFont());
  d_short_date_label=new QLabel(this);

  d_time_label_label=new QLabel(tr("Time")+":",this);
  d_time_label_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_time_label_label->setFont(labelFont());
  d_time_label=new QLabel(this);

  d_close_button=new QPushButton(tr("Close"),this);
  d_close_button->setFont(buttonFont());
  connect(d_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize TestDatetimes::sizeHint() const
{
  return QSize(400,150);
}


int TestDatetimes::exec(const QString &long_date_fmt,\
		       const QString &short_date_fmt,
		       const QString &time_fmt)
{
  d_long_date_label->setText(d_sample_datetime.toString(long_date_fmt));
  d_short_date_label->setText(d_sample_datetime.toString(short_date_fmt));
  d_time_label->setText(d_sample_datetime.toString(time_fmt));

  return QDialog::exec();
}


void TestDatetimes::closeData()
{
  done(true);
}


void TestDatetimes::resizeEvent(QResizeEvent *e)
{
  d_sample_datetime_label_label->setGeometry(10,2,120,20);
  d_sample_datetime_label->setGeometry(135,2,size().width()-145,20);

  d_long_date_label_label->setGeometry(10,30,120,20);
  d_long_date_label->setGeometry(135,30,size().width()-145,20);

  d_short_date_label_label->setGeometry(10,50,120,20);
  d_short_date_label->setGeometry(135,50,size().width()-145,20);

  d_time_label_label->setGeometry(10,70,120,20);
  d_time_label->setGeometry(135,70,size().width()-145,20);

  d_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
