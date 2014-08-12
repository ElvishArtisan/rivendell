//   rdpushbutton.cpp
//
//   An flashing button widget.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdpushbutton.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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
#include <qpushbutton.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpointarray.h>
#include <qtimer.h>
#include <qpalette.h>

#include <rdpushbutton.h>


RDPushButton::RDPushButton(QWidget *parent=0,const char *name) :
  QPushButton(parent,name)
{
  Init();
}


RDPushButton::RDPushButton(const QString &text,QWidget *parent,
			   const char *name)
  : QPushButton(text,parent,name)
{
  Init();
}

RDPushButton::RDPushButton(const QIconSet &icon,const QString &text,
			 QWidget *parent,const char *name)
  : QPushButton(text,parent,name)
{
  Init();
}


QColor RDPushButton::flashColor() const
{
  return flash_color;
}


void RDPushButton::setFlashColor(QColor color)
{
  int h=0;
  int s=0;
  int v=0;

  flash_color=color;  
  flash_palette=QPalette(QColor(flash_color),backgroundColor());

  color.getHsv(&h,&s,&v);
  if((h>180)&&(h<300)) {
    v=255;
  }
  else {
    if(v<168) {
      v=255;
    }
    else {
      v=0;
    }
  }
  s=0;
  color.setHsv(h,s,v);
  flash_palette.setColor(QPalette::Active,QColorGroup::ButtonText,color);
  flash_palette.setColor(QPalette::Inactive,QColorGroup::ButtonText,color);
}


bool RDPushButton::flashingEnabled() const
{
  return flashing_enabled;
}


void RDPushButton::setFlashingEnabled(bool state)
{
  flashing_enabled=state;
  if(flashing_enabled) {
    flashOn();
  }
  else {
    flashOff();
  }
}


int RDPushButton::id() const
{
  return button_id;
}


void RDPushButton::setId(int id)
{
  button_id=id;
}


void RDPushButton::setPalette(const QPalette &pal)
{
  off_palette=pal;
  QPushButton::setPalette(pal);
}


void RDPushButton::mousePressEvent(QMouseEvent *e)
{
  switch(e->button()) {
      case QMouseEvent::LeftButton:
	QPushButton::mousePressEvent(e);
	break;
	
      case QMouseEvent::MidButton:
	emit centerPressed();
	break;
	
      case QMouseEvent::RightButton:
	emit rightPressed();
	break;

      default:
	break;
  }
}


void RDPushButton::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
      case QMouseEvent::LeftButton:
	QPushButton::mouseReleaseEvent(e);
	break;
	
      case QMouseEvent::MidButton:
	e->accept();
	emit centerReleased();
	if((e->x()>=0)&&(e->x()<geometry().width())&&
	   (e->y()>=0)&&(e->y()<geometry().height())) {
	  emit centerClicked();
	  emit centerClicked(button_id,QPoint(e->x(),e->y()));
	}
	break;
	
      case QMouseEvent::RightButton:
	e->accept();
	emit rightReleased();
	if((e->x()>=0)&&(e->x()<geometry().width())&&
	   (e->y()>=0)&&(e->y()<geometry().height())) {
	  emit rightClicked();
	  emit rightClicked(button_id,QPoint(e->x(),e->y()));
	}
	break;

      default:
	break;
  }
}


int RDPushButton::flashPeriod() const
{
  return flash_period;
}


void RDPushButton::setFlashPeriod(int period)
{
  flash_period=period;
  if(flash_timer->isActive()) {
    flash_timer->changeInterval(flash_period);
  }
}


RDPushButton::ClockSource RDPushButton::clockSource() const
{
  return flash_clock_source;
}


void RDPushButton::setClockSource(ClockSource src)
{
  if(src==flash_clock_source) {
    return;
  }
  flash_clock_source=src;
  if((src==RDPushButton::ExternalClock)&&(flash_timer->isActive())) {
    flash_timer->stop();
  }
  if((src==RDPushButton::InternalClock)&&flashing_enabled) {
    flashOn();
  }
}


void RDPushButton::tickClock()
{
  if(!flashing_enabled) {
    return;
  }
  QKeySequence a=accel();
  if(flash_state) {
    flash_state=false;
    QPushButton::setPalette(flash_palette);
  }
  else {
    flash_state=true;
    QPushButton::setPalette(off_palette);
  }
  setAccel(a);
}


void RDPushButton::tickClock(bool state)
{
  if(!flashing_enabled) {
    return;
  }
  QKeySequence a=accel();
  if(state) {
    flash_state=false;
    QPushButton::setPalette(flash_palette);
  }
  else {
    flash_state=true;
    QPushButton::setPalette(off_palette);
  }
  setAccel(a);
}


void RDPushButton::flashOn()
{
  if((!flash_timer->isActive())&&
     (flash_clock_source==RDPushButton::InternalClock)) {
    flash_timer->start(flash_period);
  }
}


void RDPushButton::flashOff()
{
  if(flash_timer->isActive()&&
     (flash_clock_source==RDPushButton::InternalClock)) {
    flash_timer->stop();
  }
  setPalette(off_palette);
}


void RDPushButton::Init()
{
  flash_timer=new QTimer();
  connect(flash_timer,SIGNAL(timeout()),this,SLOT(tickClock()));
  flash_state=true;
  flashing_enabled=false;
  off_palette=palette();
  flash_clock_source=RDPushButton::InternalClock;
  flash_period=RDPUSHBUTTON_DEFAULT_FLASH_PERIOD;
  setFlashColor(RDPUSHBUTTON_DEFAULT_FLASH_COLOR);
  button_id=-1;
}


