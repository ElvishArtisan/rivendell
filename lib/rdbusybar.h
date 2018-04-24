//   rdbusybar.h
//
//   A 'progress bar' widget that shows busy state.
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDBUSYBAR_H
#define RDBUSYBAR_H

#include <qwidget.h>
#include <q3frame.h>
#include <qlabel.h>
#include <qtimer.h>

class RDBusyBar : public Q3Frame
{
  Q_OBJECT

 public:
  RDBusyBar(QWidget *parent=0,Qt::WFlags f=0);

 public slots:
  void activate(bool state);
  void strobe();

 private:
  void Update();
  QLabel *bar_label;
  int bar_pos;
  QTimer *bar_timer;
};


#endif  // RDBUSYBAR_H
