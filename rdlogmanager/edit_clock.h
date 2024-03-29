// edit_clock.h
//
// Edit A Rivendell Log Clock
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

#ifndef EDIT_CLOCK_H
#define EDIT_CLOCK_H

#include <QStringList>
#include <QTextEdit>

#include <rdclockmodel.h>
#include <rddialog.h>
#include <rdschedruleslist.h>

#include "clocklistview.h"

//
// Layout
//
#define CLOCK_EDGE 624
#define PIE_X_MARGIN 100
#define PIE_Y_MARGIN 125

class EditClock : public RDDialog
{
 Q_OBJECT
 public:
 EditClock(QString clockname,bool new_clock,QStringList *new_clocks,
	    QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectionChangedData(const QItemSelection &before,
			    const QItemSelection &after);
  void addData();
  void cloneData();
  void editData();
  void deleteData();
  void svcData();
  void schedRules();
  void saveData();
  void saveAsData();
  void doubleClickedData(const QModelIndex &index);
  void colorData();
  void editEventData(int line);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void Save();
  void UpdateClock(int line=-1);
  void CopyClockPerms(QString old_name,QString new_name);
  void AbandonClock(QString name);
  bool ValidateCode();
  ClockListView *edit_clocks_view;
  RDClockModel *edit_clocks_model;
  QPushButton *edit_add_button;
  QPushButton *edit_clone_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_color_button;
  QColor edit_color;
  QLabel *edit_clockname_label;
  QLabel *edit_shortname_label;
  QLineEdit *edit_shortname_edit;
  QLabel *edit_clock_label;
  QFont *edit_title_font;
  QFontMetrics *edit_title_metrics;
  bool edit_modified;
  QString edit_name;
  bool edit_new_clock;
  QStringList *edit_new_clocks;
  RDSchedRulesList* sched_rules_list;
  QLabel *edit_remarks_label;
  QTextEdit *edit_remarks_edit;
  QPushButton *edit_scheduler_rules_button;
  QPushButton *edit_save_button;
  QPushButton *edit_saveas_button;
  QPushButton *edit_services_list_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // EDIT_CLOCK_H
