// soundpanel.cpp
//
// RDSoundPanel panel for RDAirPlay
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

#include <QPainter>

#include "colors.h"
#include "soundpanel.h"

SoundPanel::SoundPanel(RDEventPlayer *player,RDCartDialog *cart_dialog,
		       bool dump_panel_updates,QWidget *parent)
  : RDWidget(parent)
{
  d_panel=
    new RDSoundPanel(rda->airplayConf()->panels(RDAirPlayConf::StationPanel),
		     rda->airplayConf()->panels(RDAirPlayConf::UserPanel),
		     rda->airplayConf()->flashPanel(),
		     "RDAirPlay",
		     rda->airplayConf()->buttonLabelTemplate(),
		     false,
		     player,cart_dialog,this);
  d_panel->setDumpPanelUpdates(dump_panel_updates);
}


SoundPanel::~SoundPanel()
{
  delete d_panel;
}


RDSoundPanel *SoundPanel::soundPanelWidget() const
{
  return d_panel;
}


void SoundPanel::resizeEvent(QResizeEvent *e)
{
  d_panel->setGeometry(5,5,size().width()-10,size().height()-10);
}


void SoundPanel::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->fillRect(0,0,size().width(),size().height(),
	      QColor(AIR_WIDGET_BACKGROUND_COLOR));
  delete p;
}
