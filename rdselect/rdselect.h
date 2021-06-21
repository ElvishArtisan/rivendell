// rdselect.h
//
// System Selector for Rivendell
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSELECT_H
#define RDSELECT_H

#include <QListWidget>
#include <QPushButton>

#include <rdconfig.h>
#include <rdmainwindow.h>
#include <rdmonitor_config.h>
#include <rdwidget.h>

class MainWidget : public RDMainWindow
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void doubleClickedData(const QModelIndex &index);
  void okData();
  void cancelData();
  
 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void SetSystem(int id);
  void SetCurrentItem(int id);
  std::vector<RDConfig *> select_configs;
  QStringList select_filenames;
  int select_current_id;
  QLabel *select_current_label;
  QLabel *select_label;
  QListWidget *select_box;
  QPixmap *login_rivendell_map;
  QPushButton *ok_button;
  QPushButton *cancel_button;
  QPixmap *greencheckmark_map;
  QPixmap *redx_map;
  RDMonitorConfig *monitor_config;
};


#endif  // RDSELECT_H
