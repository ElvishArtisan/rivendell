// edit_clock.h
//
// Edit A Rivendell Log Clock
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_CLOCK_H
#define EDIT_CLOCK_H


#include <qlabel.h>
#include <qtextedit.h>

#include <rdclock.h>
#include <rddialog.h>
#include <rdschedruleslist.h>

#include "clock_listview.h"

//
// Layout
//
#define CENTER_LINE 400
#define PIE_X_MARGIN 100
#define PIE_Y_MARGIN 125

class EditClock : public RDDialog
{
 Q_OBJECT
 public:
 EditClock(QString clockname,bool new_clock,std::vector<QString> *new_clocks,
	    QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectionChangedData(Q3ListViewItem *);
  void addData();
  void cloneData();
  void editData();
  void deleteData();
  void svcData();
  void schedRules();
  void saveData();
  void saveAsData();
  void doubleClickedData(Q3ListViewItem *,const QPoint &,int);
  void colorData();
  void editEventData(int line);
  void okData();
  void cancelData();

 protected:
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void Save();
  void RefreshList(int select_line=-1);
  void RefreshNames();
  void UpdateClock(int line=-1);
  void CopyClockPerms(QString old_name,QString new_name);
  void AbandonClock(QString name);
  bool ValidateCode();
  ClockListView *edit_clocks_list;
  RDClock *edit_clock;
  QPushButton *edit_add_button;
  QPushButton *edit_clone_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_color_button;
  QLabel *edit_clockname_label;
  QLineEdit *edit_shortname_edit;
  QLabel *edit_clock_label;
  QFont *edit_title_font;
  QFontMetrics *edit_title_metrics;
  bool edit_modified;
  QString edit_name;
  bool edit_new_clock;
  std::vector<QString> *edit_new_clocks;
  RDSchedRulesList* sched_rules_list;
  QTextEdit *edit_remarks_edit;
};


#endif  // EDIT_CLOCK_H
