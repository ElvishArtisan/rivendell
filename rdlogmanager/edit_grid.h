// edit_grid.h
//
// Edit A Rivendell Log Grid
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_grid.h,v 1.8.8.1 2013/10/11 17:16:49 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpopupmenu.h>

#include <rdpushbutton.h>

#include <rdlistview.h>
#include <rdclock.h>


class EditGrid : public QDialog
{
 Q_OBJECT
 public:
  EditGrid(QString servicename,QWidget *parent=0,const char *name=0);
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
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void LoadButtons();
  void LabelButton(int dayofweek,int hour,QString code);
  QString GetClock(int dayofweek,int hour);
  QString edit_servicename;
  QString current_clockname;
  RDPushButton *edit_hour_button[7][24];
  QPopupMenu *edit_right_menu;
  int edit_rightclick_id;
};


#endif
