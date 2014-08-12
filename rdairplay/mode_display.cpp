// mode_display.cpp
//
// The mode display widget for RDAirPlay in Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: mode_display.cpp,v 1.17.6.1 2014/02/10 20:45:13 cvs Exp $
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

#include <qpainter.h>

#include <mode_display.h>
#include <colors.h>

ModeDisplay::ModeDisplay(QWidget *parent)
  : QPushButton(parent)
{
  for(int i=0;i<RDAIRPLAY_LOG_QUANTITY;i++) {
    mode_mode[i]=RDAirPlayConf::Previous;
  }

  //
  // Generate Fonts
  //
  mode_large_font=QFont("Helvetica",26,QFont::Normal);
  mode_large_font.setPixelSize(26);
  mode_small_font=QFont("Helvetica",12,QFont::Normal);
  mode_small_font.setPixelSize(12);
  mode_tiny_font=QFont("Helvetica",10,QFont::Normal);
  mode_tiny_font.setPixelSize(10);

  //
  // Create Palettes
  //
  auto_color=
    QPalette(QColor(BUTTON_MODE_AUTO_COLOR),backgroundColor());
  live_assist_color=
    QPalette(QColor(BUTTON_MODE_LIVE_ASSIST_COLOR),backgroundColor());
  manual_color=
    QPalette(QColor(BUTTON_MODE_MANUAL_COLOR),backgroundColor());

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
  QPixmap *pix=new QPixmap(sizeHint().width(),sizeHint().height());
  QPainter *p=new QPainter(pix);
  if(mode_style==RDAirPlayConf::Unified) {
    switch(mode_mode[0]) {
    case RDAirPlayConf::LiveAssist:
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_LIVE_ASSIST_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("LiveAssist")))/2,
		  48,tr("LiveAssist"));
      setPalette(live_assist_color);
      break;

    case RDAirPlayConf::Auto:
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_AUTO_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("Automatic")))/2,
		  48,tr("Automatic"));
      setPalette(auto_color);
      break;

    case RDAirPlayConf::Manual:
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_MANUAL_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  22,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("Manual")))/2,
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
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_LIVE_ASSIST_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("LiveAssist")))/2,
		  38,tr("LiveAssist"));
      setPalette(live_assist_color);
      break;

    case RDAirPlayConf::Auto:
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_AUTO_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("Automatic")))/2,
		  39,tr("Automatic"));
      setPalette(auto_color);
      break;

    case RDAirPlayConf::Manual:
      p->fillRect(0,0,sizeHint().width(),sizeHint().height(),
		  BUTTON_MODE_MANUAL_COLOR);
      p->setPen(QColor(color1));
      p->setFont(mode_small_font);
      p->drawText((sizeHint().width()-p->fontMetrics().
		   width(tr("Operating Mode")))/2,
		  12,tr("Operating Mode"));
      p->setFont(mode_large_font);
      p->drawText((sizeHint().width()-p->fontMetrics().width(tr("Manual")))/2,
		  38,tr("Manual"));
      setPalette(manual_color);
      break;

    case RDAirPlayConf::Previous:
      break;
    }
    p->setFont(mode_tiny_font);
    str=tr("Aux1")+": "+RDAirPlayConf::logModeText(mode_mode[1]);
    p->drawText((sizeHint().width()/2-p->fontMetrics().width(str))/2,sizeHint().height()-5,str);
    str=tr("Aux2")+": "+RDAirPlayConf::logModeText(mode_mode[2]);
    p->drawText(sizeHint().width()/2+(sizeHint().width()/2-p->fontMetrics().width(str))/2,sizeHint().height()-5,str);
  }
  p->end();
  delete p;

  setPixmap(*pix);
  delete pix;
}
