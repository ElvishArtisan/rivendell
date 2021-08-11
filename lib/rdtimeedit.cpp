// rdtimeedit.cpp
//
// A QTimeEdit with tenth-second precision.
//
//   (C) Copyright 2003-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLineEdit>
#include <QStringList>

#include "rdapplication.h"
#include "rdtimeedit.h"

RDTimeEdit::RDTimeEdit(QWidget *parent)
  : QTimeEdit(parent)
{
  d_show_hours=true;
  d_show_tenths=false;
  d_step_enabled=StepDownEnabled|StepUpEnabled;
  d_width_variance=0;
  d_read_only=false;

  SetFormat();
}


bool RDTimeEdit::showHours() const
{
  return d_show_hours;
}


void RDTimeEdit::setShowHours(bool state)
{
  if(d_show_hours!=state) {
    d_show_hours=state;
    SetFormat();
  }
}


bool RDTimeEdit::showTenths() const
{
  return d_show_tenths;
}


void RDTimeEdit::setShowTenths(bool state)
{
  if(state!=d_show_tenths) {
    d_show_tenths=state;
    SetFormat();
  }
}


bool RDTimeEdit::isReadOnly() const
{
  return d_read_only;
}


void RDTimeEdit::setReadOnly(bool state)
{
  if(d_read_only!=state) {
    if(state) {
      setTimeRange(time(),time());
    }
    else {
      setTimeRange(QTime(),QTime());
    }
    d_read_only=state;
  }
}


QValidator::State RDTimeEdit::validate(QString &input,int &pos) const
{
  QValidator::State ret=QTimeEdit::validate(input,pos);

  if((!d_show_tenths)||(ret==QValidator::Invalid)) {
    return ret;
  }
  if(ret==QValidator::Acceptable) {
    if((input.length()!=displayFormat().length())&&
       (input.length()!=(displayFormat().length()+d_width_variance))) {
      return QValidator::Intermediate;
    }
  }
  return ret;
}


void RDTimeEdit::fixup(QString &input) const
{
  //
  // Don't allow higher precision than tenths of a second
  //
  if(d_show_tenths) {
    QStringList f0=input.split(".",QString::KeepEmptyParts);
    if(f0.size()==2) {
      QStringList f1=f0.at(1).split(" ");
      if(f1.at(0).length()>1) {
	input.replace("."+f1.at(0),"."+f1.at(0).left(1));
      }
    }
  }
  QTimeEdit::fixup(input);
}


void RDTimeEdit::stepBy(int steps)
{
  int step_size=1;
  int tenths=-1;
  int tenths_lo_limit=0;
  int tenths_hi_limit=9;
  bool ok=false;

  if(d_show_tenths) {
    if(displayFormat()==RD_TWENTYFOUR_HOUR_TENTHS_FORMAT) {
      if(lineEdit()->cursorPosition()>8) {
	tenths=lineEdit()->text().mid(9,1).toInt(&ok);
	if(!ok) {
	  return;
	}
	step_size=100;
      }
    }
    if(displayFormat()==RD_TWELVE_HOUR_TENTHS_FORMAT) {
      int offset=7;
      if(lineEdit()->text().mid(2,1)==":") {
	offset=8;
      }
      if((lineEdit()->cursorPosition()>offset)&&
	 (lineEdit()->cursorPosition()<=(offset+2))) {
	tenths=lineEdit()->text().mid(offset+1,1).toInt(&ok);
	if(!ok) {
	  return;
	}
	step_size=100;
      }
    }
    if(displayFormat()==RD_OFFSET_TENTHS_FORMAT) {
      if(lineEdit()->cursorPosition()>5) {
	tenths=lineEdit()->text().mid(6,1).toInt(&ok);
	if(!ok) {
	  return;
	}
	step_size=100;
      }
    }
    if(steps>0) {
      if(tenths>=9) {
	return;
      }
      QTimeEdit::stepBy(step_size);
      tenths_lo_limit=-1;
      tenths_hi_limit=8;
    }
    if(steps<0) {
      QTimeEdit::stepBy(-step_size);
      tenths_lo_limit=1;
      tenths_hi_limit=10;
    }
    if(tenths<0) {
      d_step_enabled=QTimeEdit::stepEnabled();
    }
    else {
      d_step_enabled=0;
      if(tenths>tenths_lo_limit) {
	d_step_enabled=d_step_enabled|StepDownEnabled;
      }
      if(tenths<tenths_hi_limit) {
	d_step_enabled=d_step_enabled|StepUpEnabled;
      }
    }
  }
  else {
    QTimeEdit::stepBy(steps);
  }
}


QAbstractSpinBox::StepEnabled RDTimeEdit::stepEnabled() const
{
  if(d_show_tenths) {
    return d_step_enabled;
  }
  return QTimeEdit::stepEnabled();
}


void RDTimeEdit::SetFormat()
{
  if(rda->system()->showTwelveHourTime()) {
    if(d_show_tenths) {
      if(d_show_hours) {
	setDisplayFormat(RD_TWELVE_HOUR_TENTHS_FORMAT);
	d_width_variance=1;
      }
      else {
	setDisplayFormat(RD_OFFSET_TENTHS_FORMAT);
	d_width_variance=0;
      }
    }
    else {
      if(d_show_hours) {
	setDisplayFormat(RD_TWELVE_HOUR_FORMAT);
	d_width_variance=0;
      }
      else {
	setDisplayFormat(RD_OFFSET_FORMAT);
	d_width_variance=0;
      }
    }
  }
  else {
    if(d_show_tenths) {
      if(d_show_hours) {
	setDisplayFormat(RD_TWENTYFOUR_HOUR_TENTHS_FORMAT);
	d_width_variance=0;
      }
      else {
	setDisplayFormat(RD_OFFSET_TENTHS_FORMAT);
	d_width_variance=0;
      }
    }
    else {
      if(d_show_hours) {
	setDisplayFormat(RD_TWENTYFOUR_HOUR_FORMAT);
	d_width_variance=0;
      }
      else {
	setDisplayFormat(RD_OFFSET_FORMAT);
	d_width_variance=0;
      }
    }
  }
}
