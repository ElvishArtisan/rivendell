// deckmon.cpp
//
// Monitor a Rivendell RDCatch Deck
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: deckmon.cpp,v 1.25 2010/07/29 19:32:36 cvs Exp $
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

#include <rd.h>
#include <rdcut.h>
#include <rdcart.h>
#include <rddb.h>
#include <deckmon.h>
#include <colors.h>
#include <globals.h>


DeckMon::DeckMon(QString station,unsigned channel,
		 QWidget *parent,const char *name)
  : QFrame(parent,name)
{
  mon_station=station;
  mon_channel=channel;

  setFrameStyle(Box|Raised);
  setLineWidth(1);
  setMidLineWidth(2);

  //
  // Generate Fonts
  //
  QFont small_font=QFont("Helvetica",6,QFont::Bold);
  small_font.setPixelSize(6);
  QFont label_font("Helvetica",12,QFont::Normal);
  label_font.setPixelSize(12);

  //
  // Station/Channel
  //
  mon_station_label=new QLabel(this,"mon_station_label");
  mon_station_label->setFont(label_font);
  if((mon_channel>0)&&(mon_channel<(MAX_DECKS+1))) {
    mon_station_label->
      setText(QString().
	      sprintf("%s : %uR",(const char *)mon_station,mon_channel));
  }
  if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
    mon_station_label->
      setText(QString().
	      sprintf("%s : %uP",(const char *)mon_station,mon_channel-128));
  }

  //
  // Monitor Button
  //
  mon_monitor_button=new QPushButton(this,"mon_monitor_button");
  mon_monitor_button->setFont(small_font);
  mon_monitor_button->setText(tr("MON"));
  mon_monitor_palette=new QPalette(QColor(BUTTON_MONITOR_FLASHING_COLOR),
				   backgroundColor());
  if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
    mon_monitor_button->hide();
  }
  connect(mon_monitor_button,SIGNAL(clicked()),
	  this,SLOT(monitorButtonData()));

  //
  // Abort Button
  //
  mon_abort_button=new QPushButton(this,"mon_abort_button");
  mon_abort_button->setFont(small_font);
  mon_abort_button->setText(tr("ABORT"));
  mon_abort_button->setDisabled(true);
  connect(mon_abort_button,SIGNAL(clicked()),this,SLOT(abortButtonData()));

  //
  // Cut
  //
  mon_cut_label=new QLabel(this,"mon_cut_label");
  mon_cut_label->setFont(label_font);

  //
  // Status
  //
  mon_status_label=new QLabel(tr("OFFLINE"),this,"mon_status_label");
  mon_status_label->setFont(label_font);

  //
  // Audio Meter
  //
  mon_left_meter=new RDPlayMeter(RDSegMeter::Right,this,"mon_left_meter");
  mon_left_meter->setMode(RDSegMeter::Peak);
  mon_left_meter->setRange(-4600,-800);
  mon_left_meter->setHighThreshold(-1600);
  mon_left_meter->setClipThreshold(-1100);
  mon_left_meter->setLabel(tr("L"));
  mon_right_meter=new RDPlayMeter(RDSegMeter::Right,this,"mon_right_meter");
  mon_right_meter->setMode(RDSegMeter::Peak);
  mon_right_meter->setRange(-4600,-800);
  mon_right_meter->setHighThreshold(-1600);
  mon_right_meter->setClipThreshold(-1100);
  mon_right_meter->setLabel(tr("R"));
}


DeckMon::~DeckMon()
{
}


QSize DeckMon::sizeHint() const
{
  return QSize(780,30);
}


QSizePolicy DeckMon::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void DeckMon::enableMonitorButton(bool state)
{
  mon_monitor_button->setEnabled(state);
}


void DeckMon::setGeometry(int x,int y,int w,int h)
{
  QFrame::setGeometry(x,y,w,h);
}


void DeckMon::setGeometry(const QRect &r)
{
  setGeometry(r.x(),r.y(),r.width(),r.height());
}


void DeckMon::setMonitor(bool state)
{
  if(state) {
    mon_monitor_button->setPalette(*mon_monitor_palette);
  }
  else {
    mon_monitor_button->setPalette(palette());
  }
}


void DeckMon::setStatus(RDDeck::Status status,int id,const QString &cutname)
{
  if(id==0) {
    mon_status_label->setText(tr("IDLE"));
    SetCutInfo(0,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setDisabled(true);
    return;
  }
  switch(status) {
      case RDDeck::Offline:
	mon_status_label->setText(tr("OFFLINE"));
	SetCutInfo(0,"");
	mon_left_meter->setPeakBar(-10000);
	mon_right_meter->setPeakBar(-10000);
	mon_abort_button->setDisabled(true);
	break;

      case RDDeck::Idle:
	mon_status_label->setText(tr("IDLE"));
	SetCutInfo(0,"");
	mon_left_meter->setPeakBar(-10000);
	mon_right_meter->setPeakBar(-10000);
	mon_abort_button->setDisabled(true);
	break;

      case RDDeck::Ready:
	mon_status_label->setText(tr("READY"));
	SetCutInfo(id,"");
	mon_left_meter->setPeakBar(-10000);
	mon_right_meter->setPeakBar(-10000);
	mon_abort_button->setEnabled(true);
	break;

      case RDDeck::Waiting:
	mon_status_label->setText(tr("WAITING"));
	SetCutInfo(id,"");
	mon_left_meter->setPeakBar(-10000);
	mon_right_meter->setPeakBar(-10000);
	mon_abort_button->setEnabled(true);
	break;

      case RDDeck::Recording:
	if((mon_channel>0)&&(mon_channel<(MAX_DECKS+1))) {
	  mon_status_label->setText(tr("RECORDING"));
	}
	if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
	  mon_status_label->setText(tr("PLAYING"));
	}
	SetCutInfo(id,cutname);
	mon_abort_button->setEnabled(true);
	break;
  }
}


void DeckMon::setLeftMeter(int level)
{
  mon_left_meter->setPeakBar(level);
}


void DeckMon::setRightMeter(int level)
{
  mon_right_meter->setPeakBar(level);
}


void DeckMon::monitorButtonData()
{
  emit monitorClicked();
}


void DeckMon::abortButtonData()
{
  emit abortClicked();
}


void DeckMon::resizeEvent(QResizeEvent *e)
{
  mon_station_label->setGeometry(10,10,150,12);
  mon_monitor_button->setGeometry(155,5,40,20);
  mon_abort_button->setGeometry(200,5,40,20);
  mon_cut_label->setGeometry(260,10,e->size().width()-580,12);
  mon_status_label->setGeometry(e->size().width()-320,10,80,12);
  mon_left_meter->setGeometry(e->size().width()-235,6,225,10);
  mon_right_meter->setGeometry(e->size().width()-235,16,225,10);
  QFrame::resizeEvent(e);
}


void DeckMon::SetCutInfo(int id,const QString &cutname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  if(id<0) {
    mon_cut_label->setText(tr("[multiple events]"));
    return;
  }
  if(id==0) {
    mon_cut_label->setText("");
    return;
  }
  sql=QString().sprintf("select DESCRIPTION from RECORDINGS where ID=%d",id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    if(cutname.isEmpty()) {
      mon_cut_label->setText(tr("[unknown cut]"));
    }
    else {
      sql=QString().sprintf("select CART.TITLE,CUTS.DESCRIPTION from \
                             CART left join CUTS \
                             on CART.NUMBER=CUTS.CART_NUMBER \
                             where CUTS.CUT_NAME=\"%s\"",
			    (const char *)cutname);
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	mon_cut_label->
	  setText(q1->value(0).toString()+"->"+q1->value(1).toString());
      }
      else {
	mon_cut_label->setText(tr("[unknown cut]"));
      }
      delete q1;
    }
    delete q;
    return;
  }
  if(q->value(0).toString().isEmpty()) {
    mon_cut_label->setText(tr("[no description]"));
    delete q;
    return;
  }
  mon_cut_label->setText(q->value(0).toString());
  delete q;
}
