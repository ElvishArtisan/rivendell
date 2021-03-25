//   catchtableview.h
//
//   Events List Widget for RDCatch
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

#ifndef CATCHTABLEVIEW_H
#define CATCHTABLEVIEW_H

#include <QAction>
#include <QMenu>

#include <rdmarkerdialog.h>
#include <rdtableview.h>

class CatchTableView : public RDTableView
{
  Q_OBJECT

 public:
  CatchTableView(QWidget *parent);

 private slots:
  void aboutToShowMenuData();
  void editAudioMenuData();

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  QMenu *d_mouse_menu;
  QAction *d_edit_audio_action;
  QString catch_cutname;
  QWidget *catch_parent;
  int d_mouse_row;
  int d_cue_card;
  int d_cue_port;
  RDMarkerDialog *d_marker_dialog;
};


#endif  // CATCHTABLEVIEW_H
