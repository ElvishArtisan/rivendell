//   rdpushbutton.cpp
//
//   A flashing button widget.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <q3pointarray.h>
#include <qtimer.h>
#include <qpalette.h>
//Added by qt3to4:
#include <QMouseEvent>

#include <rdpushbutton.h>


RDPushButton::RDPushButton(QWidget *parent=0,RDConfig *c)
  : QPushButton(parent), RDFontEngine(font(),c)
{
  Init();
}


RDPushButton::RDPushButton(const QString &text,QWidget *parent,RDConfig *c)
  : QPushButton(text,parent), RDFontEngine(font(),c)
{
  plain_text=text;
  Init();
}

RDPushButton::RDPushButton(const QIcon &icon,const QString &text,
			   QWidget *parent,RDConfig *c)
  : QPushButton(text,parent), RDFontEngine(font(),c)
{
  plain_text=text;
  Init();
}


QString RDPushButton::text() const
{
  return plain_text;
}


void RDPushButton::setText(const QString &str)
{
  plain_text=str;
  ComposeText();
}


bool RDPushButton::wordWrap() const
{
  return word_wrap_enabled;
}


void RDPushButton::setWordWrap(bool state)
{
  if(word_wrap_enabled!=state) {
    word_wrap_enabled=state;
    ComposeText();
  }
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
  case Qt::LeftButton:
    QPushButton::mousePressEvent(e);
    break;
	
  case Qt::MidButton:
    emit centerPressed();
    break;
	
  case Qt::RightButton:
    emit rightPressed();
    break;

  default:
    break;
  }
}


void RDPushButton::mouseReleaseEvent(QMouseEvent *e)
{
  switch(e->button()) {
  case Qt::LeftButton:
    QPushButton::mouseReleaseEvent(e);
    break;
	
  case Qt::MidButton:
    e->accept();
    emit centerReleased();
    if((e->x()>=0)&&(e->x()<geometry().width())&&
       (e->y()>=0)&&(e->y()<geometry().height())) {
      emit centerClicked();
      emit centerClicked(button_id,QPoint(e->x(),e->y()));
    }
    break;
	
  case Qt::RightButton:
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


void RDPushButton::ComposeText()
{
  int lines;
  QStringList f0=plain_text.split(" ",QString::SkipEmptyParts);
  QFont font(buttonFont().family(),(double)size().height()/2.0,QFont::Bold);
  QString accum;
  QString text;
  int height;
  bool singleton;
  int w=90*size().width()/100;
  int h=90*size().height()/100;

  if(word_wrap_enabled) {
    do {
      singleton=false;
      accum="";
      text="";
      font=QFont(font.family(),font.pointSize()-2,QFont::Bold);
      QFontMetrics fm(font);
      lines=1;
      for(int i=0;i<f0.size();i++) {
	if((fm.width(accum+f0.at(i)+" "))>w) {
	  if(fm.width(f0.at(i))>w) {
	    singleton=true;
	    break;
	  }
	  lines++;
	  accum=f0.at(i)+" ";
	  text+="\n";
	}
	else {
	  accum+=f0.at(i)+" ";
	}
	text+=f0.at(i)+" ";
      }
      height=lines*fm.lineSpacing();
    } while(singleton||(((height>h))&&(font.pointSize()>6)));
    QPushButton::setText(text.trimmed());
    QPushButton::setFont(font);
  }
  else {
    QPushButton::setText(plain_text);
  }
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
  word_wrap_enabled=false;
  button_id=-1;
}


