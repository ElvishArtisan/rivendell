//   rdmeterstrip.cpp
//
//   A strip of side-by-side vertical audio meters.
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
//

#include <math.h>

#include "rdmeterstrip.h"

RDMeterStrip::RDMeterStrip(QWidget *parent)
  : RDWidget(parent)
{
  d_poll_timer=new QTimer(this);
  connect(d_poll_timer,SIGNAL(timeout()),this,SLOT(pollData()));
  d_poll_timer->start(RD_METER_UPDATE_INTERVAL);
}


QSize RDMeterStrip::sizeHint() const
{
  return QSize(40*d_types.size(),130);
}


QSizePolicy RDMeterStrip::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
}


void RDMeterStrip::addInputMeter(int cardnum,int portnum,const QString &label)
{
  d_types.push_back(RDMeterStrip::Input);
  d_card_numbers.push_back(cardnum);
  d_port_numbers.push_back(portnum);

  AddMeter(cardnum,portnum,label);
}


void RDMeterStrip::addOutputMeter(int cardnum,int portnum,const QString &label)
{
  d_types.push_back(RDMeterStrip::Output);
  d_card_numbers.push_back(cardnum);
  d_port_numbers.push_back(portnum);

  AddMeter(cardnum,portnum,label);
}


void RDMeterStrip::pollData()
{
  short lvls[2];
  
  for(int i=0;i<d_types.size();i++) {
    switch(d_types.at(i)) {
    case RDMeterStrip::Input:
      rda->cae()->
	inputMeterUpdate(d_card_numbers.at(i),d_port_numbers.at(i),lvls);
      break;

    case RDMeterStrip::Output:
      rda->cae()->
	outputMeterUpdate(d_card_numbers.at(i),d_port_numbers.at(i),lvls);
      break;
    }
    d_meters.at(2*i)->setPeakBar(lvls[0]);
    d_meters.at(2*i+1)->setPeakBar(lvls[1]);
  }
}


void RDMeterStrip::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();
  int margin_x=(w-sizeHint().width())/2;
  if(margin_x<0) {
    margin_x=0;
  }
  int slot_w=sizeHint().width()/d_types.size();
  int meter_w=sizeHint().width()/(d_types.size()*3);
  
  for(int i=0;i<d_card_numbers.size();i++) {
    d_meters.at(2*i)->setGeometry(margin_x+i*slot_w+slot_w/5,
				  0,
				  meter_w,
				  h-20);
    d_meters.at(2*i+1)->setGeometry(margin_x+i*slot_w+slot_w/2,
				    0,
				    meter_w,
				    h-20);
    d_labels.at(i)->setGeometry(margin_x+i*slot_w,
				h-20,
				slot_w,
				20);
  }
}


void RDMeterStrip::paintEvent(QPaintEvent *)
{
  QPainter *p=new QPainter(this);

  p->fillRect(0,0,size().width(),size().height(),Qt::black);

  delete p;
}


void RDMeterStrip::AddMeter(int cardnum,int portnum,const QString &label)
{
  d_labels.push_back(new QLabel(label,this));
  d_labels.back()->setAlignment(Qt::AlignCenter);
  d_labels.back()->setFont(labelFont());
  d_labels.back()->setStyleSheet("background-color: #000000;color: #FFFFFF");
  d_labels.back()->show();
  
  d_meters.push_back(new RDPlayMeter(RDSegMeter::Up,this));
  d_meters.back()->setMode(RDSegMeter::Peak);
  d_meters.back()->setFocusPolicy(Qt::NoFocus);
  d_meters.back()->setLabel(tr("L"));
  d_meters.back()->setRange(-4600,-800);
  d_meters.back()->setHighThreshold(-1600);
  d_meters.back()->setClipThreshold(-1100);
  d_meters.back()->setSegmentSize(3);
  d_meters.back()->setSegmentGap(1);
  d_meters.back()->show();

  d_meters.push_back(new RDPlayMeter(RDSegMeter::Up,this));
  d_meters.back()->setMode(RDSegMeter::Peak);
  d_meters.back()->setFocusPolicy(Qt::NoFocus);
  d_meters.back()->setLabel(tr("R"));
  d_meters.back()->setRange(-4600,-800);
  d_meters.back()->setHighThreshold(-1600);
  d_meters.back()->setClipThreshold(-1100);
  d_meters.back()->setSegmentSize(3);
  d_meters.back()->setSegmentGap(1);
  d_meters.back()->show();

  setMinimumSize(sizeHint());
}
