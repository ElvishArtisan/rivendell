// topstrip.cpp
//
// Top row of indicator widgets for rdairplay(1)
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

#include <rdapplication.h>
#include <rd.h>

#include "colors.h"
#include "topstrip.h"

TopStrip::TopStrip(QWidget *parent)
  : RDWidget(parent)
{
  d_onair_flag=false;

  //
  // Wall Clock Widget
  //
  d_wall_clock_widget=new WallClock(this);
  d_wall_clock_widget->setCheckSyncEnabled(rda->airplayConf()->checkTimesync());

  //
  // Mode Display Widget
  //
  d_mode_display_widget=new ModeDisplay(this);
  d_mode_display_widget->setOpModeStyle(rda->airplayConf()->opModeStyle());

  //
  // Audio Meters
  //
  d_meter_widget=new RDMeterStrip(this);
  
  //
  // Message Display
  //
  d_message_widget=new QLabel(this);
  d_message_widget->setStyleSheet("background-color: "+
				   QColor(LOGLINEBOX_BACKGROUND_COLOR).name());
  d_message_widget->setWordWrap(true);
  d_message_widget->setLineWidth(1);
  d_message_widget->setMidLineWidth(1);
  d_message_widget->setFrameStyle(QFrame::Box|QFrame::Raised);
  d_message_widget->setAlignment(Qt::AlignCenter);

  //
  // Logo
  //
  QPixmap pix(rda->airplayConf()->logoPath());
  if(pix.isNull()) {
    pix.load("/usr/share/pixmaps/rivendell/rdairplay_logo.png");
  }
  d_logo=new QLabel(this);
  d_logo->setAlignment(Qt::AlignCenter);
  d_logo->setPixmap(pix);

  setFocusPolicy(Qt::NoFocus);
}


TopStrip::~TopStrip()
{
  delete d_wall_clock_widget;
  delete d_mode_display_widget;
  delete d_meter_widget;
  delete d_message_widget;
  delete d_logo;
}


QSize TopStrip::sizeHint() const
{
  return QSize(400,130);
}


QSizePolicy TopStrip::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,
		     QSizePolicy::MinimumExpanding);
}


WallClock *TopStrip::wallClockWidget() const
{
  return d_wall_clock_widget;
}


ModeDisplay *TopStrip::modeDisplayWidget() const
{
  return d_mode_display_widget;
}


RDMeterStrip *TopStrip::meterWidget()
{
  return d_meter_widget;
}


QLabel *TopStrip::messageWidget() const
{
  return d_message_widget;
}


void TopStrip::setOnairFlag(bool state)
{
  if(state!=d_onair_flag) {
    d_onair_flag=state;
    update();
  }
}


void TopStrip::resizeEvent(QResizeEvent *e)
{
  d_wall_clock_widget->setGeometry(10,5,
				   d_wall_clock_widget->sizeHint().width(),
				   d_wall_clock_widget->sizeHint().height());

  d_mode_display_widget->
    setGeometry(10,
		70,
		d_mode_display_widget->sizeHint().width(),
		d_mode_display_widget->sizeHint().height());

  d_meter_widget->setGeometry(20+d_wall_clock_widget->geometry().width(),
			      5,
			      d_meter_widget->sizeHint().width(),
			      d_meter_widget->sizeHint().height());
  
  d_message_widget->setGeometry(10+d_meter_widget->geometry().x()+
				d_meter_widget->geometry().width(),
				 5,
				size().width()-(30+d_meter_widget->geometry().x()+d_meter_widget->geometry().width()+RD_RDAIRPLAY_LOGO_WIDTH),
				 125);
  d_logo->setGeometry(size().width()-RD_RDAIRPLAY_LOGO_WIDTH-10,
		      5,
		      RD_RDAIRPLAY_LOGO_WIDTH,
		      RD_RDAIRPLAY_LOGO_HEIGHT);
}


void TopStrip::paintEvent(QPaintEvent *e)
{
  if(d_onair_flag) {
    QPainter *p=new QPainter(this);
    p->fillRect(0,0,size().width(),size().height(),PIE_ONAIR_COLOR);
    delete p;
  }
}
