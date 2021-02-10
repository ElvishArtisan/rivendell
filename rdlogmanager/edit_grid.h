// edit_grid.h
//
// Edit A Rivendell Log Grid
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

#ifndef EDIT_GRID_H
#define EDIT_GRID_H

#include <QAction>
#include <QGroupBox>
#include <QMenu>

#include <rddialog.h>
#include <rdpushbutton.h>

class EditGrid : public RDDialog
{
 Q_OBJECT
 public:
  EditGrid(QString servicename,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void hourButtonData(int id);
  void allHourButtonData();
  void rightHourButtonData(int id,const QPoint &pt);
  void aboutToShowData();
  void editClockData();
  void clearHourData();
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void LoadButtons();
  void LabelButton(int dayofweek,int hour,QString code);
  QString GetClock(int dayofweek,int hour);
  QString edit_servicename;
  QString current_clockname;
  QGroupBox *edit_day_boxes[7];
  RDPushButton *edit_hour_button[7][24];
  int edit_rightclick_id;
  QMenu *edit_mouse_menu;
  QAction *edit_edit_clock_action;
};


#endif  // EDIT_GRID_H
