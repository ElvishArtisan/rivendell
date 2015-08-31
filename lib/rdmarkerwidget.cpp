//   rdmarkerwidget.cpp
//
//   Pointer Control Widget
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <qstringlist.h>

#include "rd.h"
#include "rdmarkerwidget.h"

RDMarkerWidget::RDMarkerWidget(const QString &caption,const QColor &color,
			       unsigned samprate,QWidget *parent)
  : QWidget(parent)
{
  mark_delete_mode=false;
  mark_value=0;
  mark_sample_rate=samprate;
  mark_lo_limit=-1;
  mark_hi_limit=-1;
  mark_lo_marker=NULL;
  mark_hi_marker=NULL;

  QFont button_font=QFont("Hevetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont label_font=QFont("Hevetica",12,QFont::Normal);
  label_font.setPixelSize(12);

  mark_edit=new RDMarkerEdit(this);
  mark_edit->setGeometry(68,11,95,21);
  mark_edit->setReadOnly(true);
  mark_edit->setDragEnabled(false);
  mark_edit->setFont(label_font);
  connect(mark_edit,SIGNAL(returnPressed()),this,SLOT(returnPressedData()));
  connect(mark_edit,SIGNAL(escapePressed()),this,SLOT(escapePressedData()));

  mark_button=new RDMarkerButton(this);
  mark_button->setToggleButton(true);
  mark_button->setGeometry(0,0,66,45);
  mark_button->setFlashColor(backgroundColor());
  mark_button->setFlashPeriod(RDMARKERWIDGET_BUTTON_FLASH_PERIOD);
  mark_button->setPalette(QPalette(color,parent->backgroundColor()));
  mark_button->setFont(button_font);
  mark_button->setText(caption);
  connect(mark_button,SIGNAL(clicked()),this,SLOT(buttonClickedData()));
}


RDMarkerWidget::~RDMarkerWidget()
{
  delete mark_button;
  delete mark_edit;
}


void RDMarkerWidget::setDeleteMode(bool state)
{
  mark_delete_mode=state;
}


bool RDMarkerWidget::isSelected() const
{
  return mark_button->isOn();
}


void RDMarkerWidget::setSelected(bool state)
{
  if(!state) {
    if(mark_value>=0) {
      mark_edit->setText(GetTimeLength(mark_value));
    }
  }
  mark_button->setOn(state);
  mark_button->setFlashingEnabled(state);
  mark_edit->setReadOnly(!state);
}


int RDMarkerWidget::frames() const
{
  if(mark_value<0) {
    return -1;
  }
  return (int)(((double)mark_value*(double)mark_sample_rate)/1152000.0);
}


void RDMarkerWidget::setFrames(int frames)
{
  if(frames<0) {
    setValue(-1);
  }
  else {
    setValue((double)frames*1152000.0/(double)mark_sample_rate);
  }
}


int RDMarkerWidget::value() const
{
  return mark_value;
}


void RDMarkerWidget::setValue(int msecs)
{
  if(msecs<0) {
    mark_value=-1;
    mark_edit->setText("");
  }
  else {
    mark_value=msecs;
    mark_edit->setText(GetTimeLength(mark_value));
  }
}


void RDMarkerWidget::setRange(int lo_limit,int hi_limit)
{
  mark_lo_limit=lo_limit;
  mark_hi_limit=hi_limit;
}


void RDMarkerWidget::setRange(RDMarkerWidget *lo_limit,RDMarkerWidget *hi_limit)
{
  mark_lo_marker=lo_limit;
  mark_hi_marker=hi_limit;
}


void RDMarkerWidget::returnPressedData()
{
  int value=SetTimeLength(mark_edit->text());
  if(!CheckLimits(value)) {
    return;
  }
  if(value!=mark_value) {
    mark_value=value;
    emit valueChanged();
  }
}


void RDMarkerWidget::escapePressedData()
{
  mark_edit->setText(GetTimeLength(mark_value));
  mark_edit->selectAll();
}


void RDMarkerWidget::buttonClickedData()
{
  if(mark_delete_mode) {
    mark_button->setOn(false);
    emit deleteClicked();
  }
  else {
    if(!mark_button->isOn()) {
      returnPressedData();
    }
    mark_button->setFlashingEnabled(mark_button->isOn());
    mark_edit->setReadOnly(!mark_button->isOn());
    emit selectionChanged();
  }
}


bool RDMarkerWidget::CheckLimits(int value)
{
  if(mark_lo_limit>=0) {
    if(value<mark_lo_limit) {
      mark_edit->setText(GetTimeLength(mark_value));
      return false;
    }
  }
  if(mark_hi_limit>=0) {
    if(value>mark_hi_limit) {
      mark_edit->setText(GetTimeLength(mark_value));
      return false;
    }
  }
  if(mark_lo_marker!=NULL) {
    if(value<mark_lo_marker->value()) {
      mark_edit->setText(GetTimeLength(mark_value));
      return false;
    }
  }
  if(mark_hi_marker!=NULL) {
    if(value>mark_hi_marker->value()) {
      mark_edit->setText(GetTimeLength(mark_value));
      return false;
    }
  }
  return true;
}


int RDMarkerWidget::SetTimeLength(const QString &str) const
{
  QStringList f0=f0.split(":",str);

  return 3600000*f0[0].toInt()+60000*f0[1].toInt()+
    (int)(1000.0*f0[2].toDouble());
}


QString RDMarkerWidget::GetTimeLength(int msecs) const
{
  int hours=msecs/3600000;
  int minutes=(msecs-3600000*hours)/60000;
  int seconds=msecs-(3600000*hours+60000*minutes);

  return QString().sprintf("%02d:%02d:%05.2lf",
			   hours,minutes,(double)seconds/1000.0);
}
