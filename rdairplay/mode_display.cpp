// mode_display.cpp
//
// The mode display widget for RDAirPlay in Rivendell
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPainter>

#include "colors.h"
#include "mode_display.h"

ModeDisplay::ModeDisplay(QWidget *parent)
  : RDPushButton(parent)
{
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    mode_mode[i]=RDAirPlayConf::Previous;
  }

  //
  // Create Palettes
  //
  auto_color=
    QPalette(QColor(BUTTON_MODE_AUTO_COLOR),palette().color(QPalette::Background));
  live_assist_color=
    QPalette(QColor(BUTTON_MODE_LIVE_ASSIST_COLOR),palette().color(QPalette::Background));
  manual_color=
    QPalette(QColor(BUTTON_MODE_MANUAL_COLOR),palette().color(QPalette::Background));

  setPalette(live_assist_color);
}


QSize ModeDisplay::sizeHint() const
{
  return QSize(200,60);
}


QSizePolicy ModeDisplay::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ModeDisplay::setOpMode(int mach,RDAirPlayConf::OpMode mode)
{
  if(mach<0) {
    return;
  }
  if(mode==mode_mode[mach]) {
    return;
  }
  mode_mode[mach]=mode;
  WriteMap();
}


void ModeDisplay::setOpModeStyle(RDAirPlayConf::OpModeStyle style)
{
  if(mode_style==style) {
    return;
  }
  mode_style=style;
  WriteMap();
}


void ModeDisplay::WriteMap()
{
  QString str;
  QPixmap *pix=new QPixmap(sizeHint().width()-2,sizeHint().height()-2);
  QPainter *p=new QPainter(pix);

  if(mode_style==RDAirPlayConf::Unified) {
    switch(mode_mode[0]) {
    case RDAirPlayConf::LiveAssist:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_LIVE_ASSIST_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("LiveAssist")))/2,
		  48,tr("LiveAssist"));
      setPalette(live_assist_color);
      break;

    case RDAirPlayConf::Auto:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_AUTO_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("Automatic")))/2,
		  48,tr("Automatic"));
      setPalette(auto_color);
      break;

    case RDAirPlayConf::Manual:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_MANUAL_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("Manual")))/2,
		  48,tr("Manual"));
      setPalette(manual_color);
      break;

    case RDAirPlayConf::Previous:
      break;
    }
  }
  else {
    switch(mode_mode[0]) {
    case RDAirPlayConf::LiveAssist:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_LIVE_ASSIST_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("LiveAssist")))/2,
		  38,tr("LiveAssist"));
      setPalette(live_assist_color);
      break;

    case RDAirPlayConf::Auto:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_AUTO_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("Automatic")))/2,
		  39,tr("Automatic"));
      setPalette(auto_color);
      break;

    case RDAirPlayConf::Manual:
      p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		  BUTTON_MODE_MANUAL_COLOR);
      p->setPen(QColor(Qt::color1));
      p->setFont(subLabelFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(bannerFont());
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(tr("Manual")))/2,
		  38,tr("Manual"));
      setPalette(manual_color);
      break;

    case RDAirPlayConf::Previous:
      break;
    }
    p->setFont(subLabelFont());
    str=tr("A1")+": "+RDAirPlayConf::logModeText(mode_mode[1]);
    p->drawText(10,sizeHint().height()-2-5,str);
    str=tr("A2")+": "+RDAirPlayConf::logModeText(mode_mode[2]);
    p->drawText(sizeHint().width()-p->fontMetrics().width(str)-10,sizeHint().height()-2-5,str);
  }
  p->end();
  delete p;

  setIcon(*pix);
  delete pix;
}
