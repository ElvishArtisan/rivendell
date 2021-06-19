// mainwindow.cpp
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

#include "mainwindow.h"

MainWindow::MainWindow(const QString &cmdname,RDConfig *c,Qt::WindowFlags f)
  : RDMainWindow(cmdname,c,f)
{
  d_show_note_bubbles=false;
  d_drag_enabled=false;
}


bool MainWindow::showNoteBubbles() const
{
  return d_show_note_bubbles;
}


void MainWindow::setShowNoteBubbles(bool state)
{
  d_show_note_bubbles=state;
}


bool MainWindow::dragEnabled() const
{
  return d_drag_enabled;
}


void MainWindow::setDragEnabled(bool state)
{
  d_drag_enabled=state;
}


void MainWindow::loadLocalSettings(RDProfile *p)
{
  d_show_note_bubbles=p->intValue(commandName(),"ShowNoteBubbles");
  d_drag_enabled=p->intValue(commandName(),"DragEnabled");
}


void MainWindow::saveLocalSettings(FILE *f) const
{
  fprintf(f,"ShowNoteBubbles=%d\n",d_show_note_bubbles);
  fprintf(f,"DragEnabled=%d\n",d_drag_enabled);
}
