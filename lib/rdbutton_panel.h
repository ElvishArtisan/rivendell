// rdbutton_panel.h
//
// The sound panel widget for RDAirPlay
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdbutton_panel.h,v 1.10.6.2 2014/02/06 20:43:47 cvs Exp $
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

#include <qwidget.h>
#include <qdatetime.h>
#include <qlabel.h>

#include <rdairplay_conf.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdpanel_button.h>
#include <rdbutton_dialog.h>

//
// Widget Settings
//
#define PANEL_MAX_BUTTON_COLUMNS 20
#define PANEL_MAX_BUTTON_ROWS 20
#define PANEL_BUTTON_SIZE_X 88
#define PANEL_BUTTON_SIZE_Y 80


class RDButtonPanel
{
 public:
  RDButtonPanel(RDAirPlayConf::PanelType type,int panel,int cols,int rows,
		RDStation *station,bool flash,QWidget *parent);
  ~RDButtonPanel();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDPanelButton *panelButton(int row,int col) const;
  void setActionMode(RDAirPlayConf::ActionMode mode);
  void setAllowDrags(bool state);
  void setAcceptDrops(bool state);
  void hide();
  void show();
  void clear();

 private:
  RDPanelButton *panel_button[PANEL_MAX_BUTTON_ROWS][PANEL_MAX_BUTTON_COLUMNS];
  RDStation *panel_station;
  int panel_button_columns;
  int panel_button_rows;
};

#endif
