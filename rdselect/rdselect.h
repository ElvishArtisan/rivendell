// rdselect.h
//
// System Selector for Rivendell
//
//   (C) Copyright 2012-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <vector>

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <q3listbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qpixmap.h>

#include <rdconfig.h>
#include <rdmonitor_config.h>

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void doubleClickedData(Q3ListBoxItem *item);
  void okData();
  void cancelData();
  
 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void SetSystem(int id);
  void SetCurrentItem(int id);
  std::vector<RDConfig *> select_configs;
  QStringList select_filenames;
  int select_current_id;
  QLabel *select_current_label;
  QLabel *select_label;
  Q3ListBox *select_box;
  QPixmap *login_rivendell_map;
  QPushButton *ok_button;
  QPushButton *cancel_button;
  QPixmap *greencheckmark_map;
  QPixmap *redx_map;
  RDMonitorConfig *monitor_config;
};


#endif  // RDSELECT_H
