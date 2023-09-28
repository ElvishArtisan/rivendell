// rdbutton_panel.h
//
// The sound panel widget for RDAirPlay
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDBUTTON_PANEL_H
#define RDBUTTON_PANEL_H

#include <QDateTime>
#include <QLabel>

#include <rdairplay_conf.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdpanel_button.h>
#include <rdbutton_dialog.h>
#include <rdweb.h>
#include <rdwidget.h>

//
// Widget Settings
//
#define PANEL_MAX_BUTTON_COLUMNS 40
#define PANEL_MAX_BUTTON_ROWS 23
#define PANEL_BUTTON_SIZE_X 88
#define PANEL_BUTTON_SIZE_Y 80


class RDButtonPanel : public RDWidget
{
 public:
  RDButtonPanel(RDAirPlayConf::PanelType type,int number,QWidget *parent);
  ~RDButtonPanel();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  int number() const;
  QString title() const;
  void setTitle(const QString &str);
  RDPanelButton *panelButton(int row,int col) const;
  void setActionMode(RDAirPlayConf::ActionMode mode);
  void setAllowDrags(bool state);
  void setAcceptDrops(bool state);
  void hide();
  void show();
  void clear();
  QString json(int padding=0,bool final=false) const;

 private:
  int panel_number;
  QString panel_title;
  RDPanelButton *panel_button[PANEL_MAX_BUTTON_ROWS][PANEL_MAX_BUTTON_COLUMNS];
};

#endif  // RDPANEL_BUTTON_H
