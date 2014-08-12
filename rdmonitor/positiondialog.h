// positiondialog.h
//
// Dialog to set RDMonitor screen position.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: positiondialog.h,v 1.1.2.1 2013/11/08 03:57:15 cvs Exp $
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

#ifndef POSITIONDIALOG_H
#define POSITIONDIALOG_H

#include <qcombobox.h>
#include <qdesktopwidget.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <rdmonitor_config.h>

class PositionDialog : public QDialog
{
 Q_OBJECT
 public:
  PositionDialog(QDesktopWidget *dw,RDMonitorConfig *config,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  int exec();

 private slots:
  void okData();
  void cancelData();

 private:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  QLabel *pos_screen_number_label;
  QComboBox *pos_screen_number_box;
  QLabel *pos_position_label;
  QComboBox *pos_position_box;
  QLabel *pos_x_offset_label;
  QSpinBox *pos_x_offset_spin;
  QLabel *pos_y_offset_label;
  QSpinBox *pos_y_offset_spin;
  QPushButton *pos_ok_button;
  QPushButton *pos_cancel_button;
  QDesktopWidget *pos_desktop_widget;
  RDMonitorConfig *pos_config;
};


#endif  // POSITIONDIALOG_H
