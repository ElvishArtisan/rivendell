// rddatetimeedit.cpp
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

#include <rddatetimeedit.h>

RDDateTimeEdit::RDDateTimeEdit(QWidget *parent)
  : QWidget(parent)
{
  d_date_edit=new RDDateEdit(this);
  d_time_edit=new RDTimeEdit(this);
}


RDDateTimeEdit::~RDDateTimeEdit()
{
  delete d_time_edit;
  delete d_date_edit;
}


QSize RDDateTimeEdit::sizeHint() const
{
  return QSize(d_date_edit->sizeHint().width()+
	       d_time_edit->sizeHint().width(),
	       d_date_edit->sizeHint().height());
}


QSizePolicy RDDateTimeEdit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

 
QDate RDDateTimeEdit::date() const
{
  return d_date_edit->date();
}


QDateTime RDDateTimeEdit::dateTime() const
{
  return QDateTime(d_date_edit->date(),d_time_edit->time());
}


bool RDDateTimeEdit::isReadOnly()
{
  return d_date_edit->isReadOnly();
}


bool RDDateTimeEdit::showTenths() const
{
  return d_time_edit->showTenths();
}


QTime RDDateTimeEdit::time() const
{
  return d_time_edit->time();
}


void RDDateTimeEdit::setDate(const QDate &date)
{
  d_date_edit->setDate(date);
}


void RDDateTimeEdit::setDateTime(const QDateTime &dt)
{
  d_date_edit->setDate(dt.date());
  d_time_edit->setTime(dt.time());
}


void RDDateTimeEdit::setReadOnly(bool state)
{
  d_date_edit->setReadOnly(state);
  d_time_edit->setReadOnly(state);
}


void RDDateTimeEdit::setShowTenths(bool state)
{
  d_time_edit->setShowTenths(state);
}


void RDDateTimeEdit::setTime(const QTime &time)
{
  d_time_edit->setTime(time);
}


void RDDateTimeEdit::resizeEvent(QResizeEvent *e)
{
  d_date_edit->setGeometry(0,0,size().width()/2,size().height());
  d_time_edit->
    setGeometry(size().width()/2,0,size().width()/2,size().height());
}

