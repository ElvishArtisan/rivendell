//   rdtransportbutton.cpp
//
//   An audio transport button widget.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QBitmap>
#include <QPainter>
#include <QPolygon>
#include <QTimer>

#include "rdtransportbutton.h"

RDTransportButton::RDTransportButton(RDTransportButton::TransType type,
				     QWidget *parent)
  :  QPushButton(parent)
{
  button_type=type;
  button_state=RDTransportButton::Off;
  on_color=QColor(RDTRANSPORTBUTTON_DEFAULT_ON_COLOR);
  accent_color=QColor(palette().shadow().color());
  on_cap=new QPixmap();
  off_cap=new QPixmap();
  setFocusPolicy(Qt::NoFocus);

  flash_timer=new QTimer(this);
  connect(flash_timer,SIGNAL(timeout()),this,SLOT(flashClock()));
  flash_state=false;
}


RDTransportButton::TransType RDTransportButton::getType() const
{
  return button_type;
}


void RDTransportButton::setType(RDTransportButton::TransType type)
{
  button_type=type;
}


QColor RDTransportButton::onColor() const
{
  return on_color;
}


void RDTransportButton::setOnColor(QColor color)
{
  if(color!=on_color) {
    on_color=color;
    drawOnCap();
    drawOffCap();
    updateCaps();
  }
}

QColor RDTransportButton::accentColor() const
{
  return accent_color;
}


void RDTransportButton::setAccentColor(QColor color)
{
  if(color!=accent_color) {
    accent_color=color;
    drawOnCap();
    drawOffCap();
    updateCaps();
  }
}

void RDTransportButton::setState(RDTransportButton::TransState state)
{
  button_state=state;
  switch(button_state) {
      case RDTransportButton::On:
	flashOff();
	if(isEnabled()) {
	  setIcon(*on_cap);
	}
	break;
      case RDTransportButton::Off:
	flashOff();
	if(isEnabled()) {
	  setIcon(*off_cap);
	}
	break;
      case RDTransportButton::Flashing:
	if(isEnabled()) {
	  flashOn();
	}
	break;
  }
}


void RDTransportButton::on()
{
  setState(RDTransportButton::On);
}


void RDTransportButton::off()
{
  setState(RDTransportButton::Off);
}


void RDTransportButton::flash()
{
  setState(RDTransportButton::Flashing);
}


void RDTransportButton::resizeEvent(QResizeEvent *event)
{
  setIconSize(capSize());

  drawOnCap();
  drawOffCap();
  switch(button_state) {
  case RDTransportButton::Off:
    setIcon(*off_cap);
    break;
    
  case RDTransportButton::On:
    setIcon(*on_cap);
    break;
    
  default:
    setIcon(*off_cap);
    break;
  }
}


void RDTransportButton::enabledChange(bool oldEnabled)
{
  if(isEnabled()&&!oldEnabled) {
    setState(button_state);
    update();
  }
  if(!isEnabled()&&oldEnabled) {
//    setIcon(*grey_cap);
    update();
  }
}
 

void RDTransportButton::flashClock()
{
  if(flash_state) {
    flash_state=false;
    setIcon(*off_cap);
  }
  else {
    flash_state=true;
    setIcon(*on_cap);
  }
}


void RDTransportButton::updateCaps()
{
  switch(button_state) {
      case RDTransportButton::On:
	setIcon(*on_cap);
	break;

      case RDTransportButton::Flashing:
	if(flash_state) {
	  setIcon(*on_cap);
	}
	else {
	  setIcon(*off_cap);
	}
	break;

      case RDTransportButton::Off:
	setIcon(*off_cap);
	break;
  }
}


void RDTransportButton::drawMask(QPixmap *cap)
{
  QPolygon triangle=QPolygon(3);
  QPainter *p;
  QBitmap *bitmap=new QBitmap(capSize());
  int edge;

  if(capSize().height()<capSize().width()) {
    edge=capSize().height();
  }
  else {
    edge=capSize().width();
  }
  p=new QPainter(bitmap);
  p->fillRect(0,0,capSize().width(),capSize().height(),QColor(Qt::color0));
  p->setPen(QColor(Qt::color1));
  p->setBrush(QColor(Qt::color1));

  switch(button_type) {
  case RDTransportButton::Play:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Stop:
    p->fillRect(capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10,
	       edge*3/5,edge*3/5,QColor(Qt::color1));
    break;

  case RDTransportButton::Record:
    p->drawEllipse(capSize().width()/2-(3*edge)/10,
		   capSize().height()/2-(3*edge)/10,(3*edge)/5,(3*edge)/5);
    break;

  case RDTransportButton::FastForward:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Rewind:
    triangle.setPoint(0,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Eject:
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->drawPolygon(triangle);
    p->fillRect(capSize().width()/2-(3*edge)/10,capSize().height()/2+edge/10,
	       (3*edge)/5,edge/5,QColor(Qt::color1));
    break;

  case RDTransportButton::Pause:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		(3*edge)/15,(3*edge)/5,QColor(Qt::color1));
    p->fillRect(capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10,
		(3*edge)/15,(3*edge)/5,QColor(Qt::color1));
    break;

  case RDTransportButton::PlayFrom:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(Qt::color1));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::PlayBetween:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(Qt::color1));
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(Qt::color1));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Loop:
    /*
      p->moveTo(width()/2-(2*edge)/10+1,height()/2+(edge)/4);
      p->moveTo(width()/2+(edge)/10+1,height()/2-edge/10);
      p->moveTo(width()/2-(2*edge)/10+1,height()/2+(edge)/4);
    */
    p->drawArc(capSize().width()/6,capSize().height()/2-edge/9,
	       2*capSize().width()/3,
	       capSize().height()/3+edge/10,1440,5760);
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(edge)/4);
    triangle.setPoint(1,capSize().width()/2+(edge)/10+1,
		      capSize().height()/2-edge/10);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+edge/20);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Up:
    triangle.setPoint(0,capSize().width()/2,(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Down:
    triangle.setPoint(0,capSize().width()/2,capSize().height()-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::PlayTo:
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(Qt::color1));
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  default:
    p->fillRect(0,0,capSize().width(),capSize().height(),QColor(Qt::color1));
  }
  p->end();
  cap->setMask(*bitmap);
  delete bitmap;
  delete p;
}


void RDTransportButton::drawOnCap()
{
  if(capSize()==QSize(0,0)) {
    return;
  }

  QPainter *p;
  QPolygon triangle=QPolygon(3);
  int edge;

  if(capSize().height()<capSize().width()) {
    edge=capSize().height();
  }
  else {
    edge=capSize().width();
  }
  delete on_cap;
  on_cap=new QPixmap(capSize());
  on_cap->fill(palette().color(QPalette::Background));
  drawMask(on_cap);
  p=new QPainter(on_cap);
  p->setPen(on_color);
  p->setBrush(on_color);
  switch(button_type) {
  case RDTransportButton::Play:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::Stop:
    p->fillRect(capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10,
    	       edge*3/5,edge*3/5,QColor(on_color));
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-edge*3/10,capSize().height()/2+edge*3/10,
	       capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10);
    p->drawLine(capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10,
	       capSize().width()/2+edge*3/10,capSize().height()/2-edge*3/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+edge*3/10,capSize().height()/2-edge*3/10,
	       capSize().width()/2+edge*3/10,capSize().height()/2+edge*3/10);
    p->drawLine(capSize().width()/2+edge*3/10,capSize().height()/2+edge*3/10,
	       capSize().width()/2-edge*3/10,capSize().height()/2+edge*3/10);
    break;

  case RDTransportButton::Record:
    p->setPen(QColor(Qt::red));
    p->setBrush(QColor(Qt::red));
    p->drawEllipse(capSize().width()/2-(3*edge)/10,
		   capSize().height()/2-(3*edge)/10,(3*edge)/5,(3*edge)/5);
    break;

  case RDTransportButton::FastForward:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
	       capSize().width()/2,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2,
	       capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2+(3*edge)/10,
	       capSize().width()/2,capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2-(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2,
	       capSize().width()/2,capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::Rewind:
    triangle.setPoint(0,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2,capSize().height()/2);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2,
	       capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2-(3*edge)/10,
	       capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,capSize().height()/2,
	       capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2+(3*edge)/10,
	       capSize().width()/2,capSize().height()/2-(3*edge)/10);
    break;

  case RDTransportButton::Eject:
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,capSize().height()/2,
	       capSize().width()/2,capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2-(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2,
	       capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->fillRect(capSize().width()/2-(3*edge)/10,capSize().height()/2+edge/10,
	       (3*edge)/5,edge/5,on_color);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+edge/10+edge/5,
	       capSize().width()/2-(3*edge)/10,capSize().height()/2+edge/10);
    p->drawLine(capSize().width()/2-(3*edge)/10,capSize().height()/2+edge/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()/2+edge/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2+edge/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2+edge/10+edge/5);
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2+edge/10+edge/5,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+edge/10+edge/5);
    break;

  case RDTransportButton::Pause:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
	       (3*edge)/15,(3*edge)/5,on_color);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
	       capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2+(3*edge)/10);
    p->drawLine(capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->fillRect(capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10,
		(3*edge)/15,(3*edge)/5,on_color);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2+(3*edge)/30,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10);
    p->drawLine(capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/30,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::PlayFrom:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10+1,
		      capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10,
	       capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10+1,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::PlayBetween:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,
		      capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(2*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::Loop:
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,capSize().height()/2+(edge)/4,
	       capSize().width()/2+(edge)/10+1,capSize().height()/2-edge/10);
    p->setPen(QColor(palette().dark().color()));
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(edge)/10+1,capSize().height()/2-edge/10,
	       capSize().width()/2-(2*edge)/10+1,capSize().height()/2+(edge)/4);
    p->setPen(QColor(palette().shadow().color()));
    p->drawArc(capSize().width()/6,
	       capSize().height()/2-edge/9,2*capSize().width()/3,
	       capSize().height()/3+edge/10,1440,5760);
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(edge)/4);
    triangle.setPoint(1,capSize().width()/2+(edge)/10+1,
		      capSize().height()/2-edge/10);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+edge/20);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Up:
    triangle.setPoint(0,capSize().width()/2,(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,capSize().height()-(3*edge)/10,
	       capSize().width()/2,(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()-(3*edge)/10,
	       capSize().width()/2-(3*edge)/10,capSize().height()-(3*edge)/10);
    break;

  case RDTransportButton::Down:
    triangle.setPoint(0,capSize().width()/2,capSize().height()-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,(3*edge)/10,
	       capSize().width()/2,capSize().height()-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2,capSize().height()-(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,(3*edge)/10);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,(3*edge)/10,
	       capSize().width()/2-(3*edge)/10,(3*edge)/10);
    break;

  case RDTransportButton::PlayTo:
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
	       3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(2*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    break;
  }  
  p->end();

  delete p;
}


void RDTransportButton::drawOffCap()
{
  if(capSize()==QSize(0,0)) {
    return;
  }

  QPainter *p;
  QPolygon triangle=QPolygon(3);
  int edge;

  if(capSize().height()<capSize().width()) {
    edge=capSize().height();
  }
  else {
    edge=capSize().width();
  }
  delete off_cap;
  off_cap=new QPixmap(capSize());
  off_cap->fill(palette().color(QPalette::Background));
  drawMask(off_cap);
  p=new QPainter(off_cap);
  p->setPen(QColor(Qt::black));
  p->setBrush(QColor(Qt::black));
  switch(button_type) {
  case RDTransportButton::Play:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2,
	       capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::Stop:
    p->fillRect(capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10,
	       edge*3/5,edge*3/5,QColor(Qt::black));
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-edge*3/10,capSize().height()/2+edge*3/10,
	       capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10);
    p->drawLine(capSize().width()/2-edge*3/10,capSize().height()/2-edge*3/10,
	       capSize().width()/2+edge*3/10,capSize().height()/2-edge*3/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+edge*3/10,capSize().height()/2-edge*3/10,
	       capSize().width()/2+edge*3/10,capSize().height()/2+edge*3/10);
    p->drawLine(capSize().width()/2+edge*3/10,capSize().height()/2+edge*3/10,
	       capSize().width()/2-edge*3/10,capSize().height()/2+edge*3/10);
    break;

  case RDTransportButton::Record:
    p->setPen(QColor(Qt::darkRed));
    p->setBrush(QColor(Qt::darkRed));
    p->drawEllipse(capSize().width()/2-(3*edge)/10,
		   capSize().height()/2-(3*edge)/10,
		   (3*edge)/5,(3*edge)/5);
    break;

  case RDTransportButton::FastForward:
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2+(3*edge)/10,
	       capSize().width()/2,capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2-(3*edge)/10,
	       capSize().width()/2+(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,capSize().height()/2,
	       capSize().width()/2,capSize().height()/2+(3*edge)/10);
    break;
    
  case RDTransportButton::Rewind:
    triangle.setPoint(0,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2+(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2,capSize().height()/2);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2-(3*edge)/10,
	       capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,capSize().height()/2,
	       capSize().width()/2,capSize().height()/2+(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2,capSize().height()/2+(3*edge)/10,
	       capSize().width()/2,capSize().height()/2-(3*edge)/10);
    break;

  case RDTransportButton::Eject:
    triangle.setPoint(0,capSize().width()/2,capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,capSize().height()/2);
    p->drawPolygon(triangle);
    p->fillRect(capSize().width()/2-(3*edge)/10,capSize().height()/2+edge/10,
	       (3*edge)/5,edge/5,QColor(Qt::black));		   
    break;
    
  case RDTransportButton::Pause:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		(3*edge)/15,(3*edge)/5,QColor(Qt::black));
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2+(3*edge)/10);
    p->drawLine(capSize().width()/2-(3*edge)/10+(3*edge)/15,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->fillRect(capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10,
		(3*edge)/15,(3*edge)/5,QColor(Qt::black));
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2+(3*edge)/30,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10);
    p->drawLine(capSize().width()/2+(3*edge)/30,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    p->drawLine(capSize().width()/2+(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2+(3*edge)/30,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::PlayFrom:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(3*edge)/10+1,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(3*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::PlayBetween:
    p->fillRect(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(2*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(2*edge)/10+1,
		capSize().height()/2+(3*edge)/10);
    break;

  case RDTransportButton::Loop:
    triangle.setPoint(0,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2-(edge)/4);
    triangle.setPoint(1,capSize().width()/2+(edge)/10+1,
		      capSize().height()/2-edge/10);
    triangle.setPoint(2,capSize().width()/2-(2*edge)/10+1,
		      capSize().height()/2+edge/20);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(2*edge)/10+1,capSize().height()/2+(edge)/4,
	       capSize().width()/2+(edge)/10+1,capSize().height()/2-edge/10);
    p->setPen(QColor(palette().dark().color()));
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(edge)/10+1,capSize().height()/2-edge/10,
	       capSize().width()/2-(2*edge)/10+1,capSize().height()/2+(edge)/4);
    p->setPen(QColor(palette().shadow().color()));
    p->drawArc(capSize().width()/6,capSize().height()/2-edge/9,
	       2*capSize().width()/3,
	       capSize().height()/3+edge/10,1440,5760);
    break;

  case RDTransportButton::Up:
    triangle.setPoint(0,capSize().width()/2,(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()-(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::Down:
    triangle.setPoint(0,capSize().width()/2,capSize().height()-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(3*edge)/10,(3*edge)/10);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,(3*edge)/10);
    p->drawPolygon(triangle);
    break;

  case RDTransportButton::PlayTo:
    p->fillRect(capSize().width()/2+(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		3,(3*edge)/5,QBrush(accent_color));
    triangle.setPoint(0,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2-(3*edge)/10);
    triangle.setPoint(1,capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    triangle.setPoint(2,capSize().width()/2-(3*edge)/10,
		      capSize().height()/2+(3*edge)/10);
    p->drawPolygon(triangle);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(capSize().width()/2-(3*edge)/10,
		capSize().height()/2-(3*edge)/10,
		capSize().width()/2+(2*edge)/10+1,capSize().height()/2);
    p->setPen(QColor(palette().light().color()));
    p->drawLine(capSize().width()/2+(2*edge)/10+1,capSize().height()/2,
		capSize().width()/2-(3*edge)/10,
		capSize().height()/2+(3*edge)/10);
    break;
  }  
  p->end();

  delete p;
}

void RDTransportButton::flashOn()
{
  if(!flash_timer->isActive()) {
    flash_timer->start(500);
  }
}


void RDTransportButton::flashOff()
{
  if(flash_timer->isActive()) {
    flash_timer->stop();
  }
}


QSize RDTransportButton::capSize() const
{
  if(text().isEmpty()) {
    return QSize(size().width()-2,size().height()-2);
  }
  return QSize(size().width()/2,size().height()/2);
}

