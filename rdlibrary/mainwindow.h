// mainwindow.h
//
// Top-level window for rdlibrary(1)
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <rdmainwindow.h>

class MainWindow : public RDMainWindow
{
  Q_OBJECT;
 public:
  MainWindow(const QString &cmdname,RDConfig *c,Qt::WindowFlags f=0);
  bool showNoteBubbles() const;
  void setShowNoteBubbles(bool state);
  bool dragEnabled() const;
  void setDragEnabled(bool state);

 protected:
  void loadLocalSettings(RDProfile *p);
  void saveLocalSettings(FILE *f) const;

 private:
  bool d_show_note_bubbles;
  bool d_drag_enabled;
};


#endif  // MAINWINDOW_H
