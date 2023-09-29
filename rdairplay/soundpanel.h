// soundpanel.h
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

#ifndef SOUNDPANEL_H
#define SOUNDPANEL_H

#include <rdsound_panel.h>
#include <rdwidget.h>

class SoundPanel : public RDWidget
{
  Q_OBJECT
 public:
  SoundPanel(RDEventPlayer *player,RDCartDialog *cart_dialog,
	     bool dump_panel_updates,QWidget *parent);
  ~SoundPanel();
  RDSoundPanel *soundPanelWidget() const;
  
 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);

 private:
  RDSoundPanel *d_panel;
};

#endif  // SOUNDPANEL_H
